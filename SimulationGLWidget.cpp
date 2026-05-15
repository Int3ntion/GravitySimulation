#include "SimulationGLWidget.h"
#include <cmath>
#include <Windows.h>
#include <gl/GLU.h>

void Object::drawSphere() const {
    glColor3f(color[0], color[1], color[2]);
    const int slices = 64, stacks = 64;
    const float radius = static_cast<float>(this->radius);

    for (int i = 0; i < stacks; ++i) {
        float lat0 = M_PI * (-0.5f + static_cast<float>(i) / stacks);
        float z0 = std::sin(lat0);
        float r0 = std::cos(lat0);

        float lat1 = M_PI * (-0.5f + static_cast<float>(i + 1) / stacks);
        float z1 = std::sin(lat1);
        float r1 = std::cos(lat1);

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= slices; ++j) {
            float lng = 2.0f * M_PI * static_cast<float>(j) / slices;
            float x = std::cos(lng);
            float y = std::sin(lng);

            glVertex3f(
                static_cast<float>(position[0]) + radius * x * r0,
                static_cast<float>(position[1]) + radius * y * r0,
                static_cast<float>(position[2]) + radius * z0
            );
            glVertex3f(
                static_cast<float>(position[0]) + radius * x * r1,
                static_cast<float>(position[1]) + radius * y * r1,
                static_cast<float>(position[2]) + radius * z1
            );
        }
        glEnd();
    }
}

SimulationGLWidget::SimulationGLWidget(QWidget* parent)
    : QOpenGLWidget(parent) {
    setFocusPolicy(Qt::StrongFocus);
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &SimulationGLWidget::updateObjects);
    connect(m_timer, &QTimer::timeout, this, static_cast<void (QOpenGLWidget::*)()>(&QOpenGLWidget::update));

    startSimulation();
}

SimulationGLWidget::~SimulationGLWidget() {
    delete m_timer;
}

void SimulationGLWidget::initializeGL() {
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);

    // Инициализация объектов
    m_objects = {
        Object({0.0, 500.0, 0.0}, {0.0, 0.0, 0.0}, 1.98e30, 300.0, {1.0f, 0.4f, 0.0f}, "Солнце"),
        Object({-1000.0, 0.0, 0.0}, {0.0, -220000000.0, 5000000.0}, 1.972e8, 50.0, {0.0f, 0.4f, 0.5f}, "Метеор"),
        //Object({500.0, 0.0, 700.0}, {0.0, -4000000.0, -3000000.0}, 1.972e23, 50.0, {0.8f, 0.04f, 0.04f}, "Меркурий"),
        Object({-2000.0, 500.0, 0.0}, {0.0, 0.0, 220000000.0}, 5.972e24, 50.0, {0.1f, 0.2f, 0.5f}, "Земля")
    };
    calculateGravityField();
}

void SimulationGLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, static_cast<float>(w) / h, 0.1f, 40000.0f);
}

void SimulationGLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(static_cast<GLfloat>(m_cameraPitch), 1.0f, 0.0f, 0.0f);
    glRotatef(static_cast<GLfloat>(m_cameraYaw), 0.0f, 1.0f, 0.0f);

    glTranslatef(static_cast<GLfloat>(m_cameraX),
        static_cast<GLfloat>(m_cameraY),
        static_cast<GLfloat>(m_cameraZ));

    glColor3f(0.2f, 0.2f, 0.2f); // Серый цвет для гравитационной сетки
    const int gridSize = 4000;    // Размер сетки (в единицах мира)
    const int resolution = m_gridResolution; // Разрешение сетки
    const double step = (2.0 * gridSize) / resolution;
    glBegin(GL_LINES);

    // Линии по оси X (горизонтальные)
    for (int i = 0; i <= resolution; ++i) {
        for (int j = 0; j < resolution; ++j) {
            double x1 = -gridSize + i * step;
            double z1 = -gridSize + j * step;
            double y1 = m_gravityField[i][j];

            double x2 = -gridSize + i * step;
            double z2 = -gridSize + (j + 1) * step;
            double y2 = m_gravityField[i][j + 1];

            glVertex3f(static_cast<float>(x1), static_cast<float>(y1), static_cast<float>(z1));
            glVertex3f(static_cast<float>(x2), static_cast<float>(y2), static_cast<float>(z2));
        }
    }

    // Линии по оси Z (вертикальные)
    for (int j = 0; j <= resolution; ++j) {
        for (int i = 0; i < resolution; ++i) {
            double x1 = -gridSize + i * step;
            double z1 = -gridSize + j * step;
            double y1 = m_gravityField[i][j];

            double x2 = -gridSize + (i + 1) * step;
            double z2 = -gridSize + j * step;
            double y2 = m_gravityField[i + 1][j];

            glVertex3f(static_cast<float>(x1), static_cast<float>(y1), static_cast<float>(z1));
            glVertex3f(static_cast<float>(x2), static_cast<float>(y2), static_cast<float>(z2));
        }
    }
    glEnd();

    for (const auto& obj : m_objects) {
        obj.drawSphere();
    }
}

void SimulationGLWidget::calculateForces() {
    for (auto& obj : m_objects) {
        for (const auto& obj2 : m_objects) {
            if (&obj2 == &obj) continue;

            double dx = obj2.position[0] - obj.position[0];
            double dy = obj2.position[1] - obj.position[1];
            double dz = obj2.position[2] - obj.position[2];
            double distance = std::sqrt(dx * dx + dy * dy + dz * dz);
            if (distance < 150) distance = 150;

            std::vector<double> direction = { dx / distance, dy / distance, dz / distance };
            double acc1 = G * obj2.mass / (distance * distance);
            std::vector<double> acc = {
                acc1 * direction[0],
                acc1 * direction[1],
                acc1 * direction[2]
            };
            obj.accelerate(acc[0] * dt, acc[1] * dt, acc[2] * dt);
        }
    }
}

void SimulationGLWidget::updateObjects() {
    calculateForces();
    for (auto& obj : m_objects) {
        obj.updatePos(dt);
    }
    calculateGravityField();
    updateCamera();
    update(); // Перерисовка
}

void SimulationGLWidget::startSimulation() {
    m_timer->start(16); // 1000/FPS
}

void SimulationGLWidget::stopSimulation() {
    m_timer->stop();
}

void SimulationGLWidget::calculateGravityField() {
    const int size = m_gridResolution;
    const double worldSize = 4000.0;
    const double step = (2.0 * worldSize) / size;

    m_gravityField = std::vector<std::vector<double>>(size + 1, std::vector<double>(size + 1, 0.0));

    for (int i = 0; i <= size; ++i) {
        for (int j = 0; j <= size; ++j) {
            double x = -worldSize + i * step;
            double z = -worldSize + j * step;
            double totalPotential = 0.0;

            for (const auto& obj : m_objects) {
                double x0 = obj.position[0];
                double z0 = obj.position[2];

                double dx = x - x0;
                double dz = z - z0;
                double r = 700;

                double A = std::log10(obj.mass) * 5e1;
                double potential = -A * r / (std::sqrt(dx * dx + dz * dz + r * r));
                totalPotential += potential;
            }
            m_gravityField[i][j] = totalPotential - 300;
        }
    }
}

void SimulationGLWidget::keyPressEvent(QKeyEvent* event) {
    switch (event->key()) {
    case Qt::Key_W:
        m_keyWPressed = true;
        break;
    case Qt::Key_A:
        m_keyAPressed = true;
        break;
    case Qt::Key_S:
        m_keySPressed = true;
        break;
    case Qt::Key_D:
        m_keyDPressed = true;
        break;
    case Qt::Key_E:
        m_keyEPressed = true;
        break;
    case Qt::Key_Q:
        m_keyQPressed = true;
        break;
    case Qt::Key_Down:
        m_keyUpPressed = true;
        break;
    case Qt::Key_Up:
        m_keyDownPressed = true;
        break;
    case Qt::Key_Left:
        m_keyLeftPressed = true;
        break;
    case Qt::Key_Right:
        m_keyRightPressed = true;
        break;
    default:
        QOpenGLWidget::keyPressEvent(event);
    }
}

void SimulationGLWidget::keyReleaseEvent(QKeyEvent* event) {
    switch (event->key()) {
    case Qt::Key_W:
        m_keyWPressed = false;
        break;
    case Qt::Key_A:
        m_keyAPressed = false;
        break;
    case Qt::Key_S:
        m_keySPressed = false;
        break;
    case Qt::Key_D:
        m_keyDPressed = false;
        break;
    case Qt::Key_E:
        m_keyEPressed = false;
        break;
    case Qt::Key_Q:
        m_keyQPressed = false;
        break;
    case Qt::Key_Down:
        m_keyUpPressed = false;
        break;
    case Qt::Key_Up:
        m_keyDownPressed = false;
        break;
    case Qt::Key_Left:
        m_keyLeftPressed = false;
        break;
    case Qt::Key_Right:
        m_keyRightPressed = false;
        break;
    default:
        QOpenGLWidget::keyReleaseEvent(event);
    }
}

void SimulationGLWidget::updateCamera() {
    if (m_keyWPressed || m_keySPressed || m_keyAPressed || m_keyDPressed) {
        double forwardX = std::sin(-m_cameraYaw * M_PI / 180.0);
        double forwardZ = std::cos(m_cameraYaw * M_PI / 180.0);
        double leftX = std::sin((m_cameraYaw + 90.0) * M_PI / 180.0);
        double leftZ = std::cos((m_cameraYaw + 90.0) * M_PI / 180.0);
;
        if (m_keyWPressed) {
            m_cameraX += forwardX * m_cameraSpeed;
            m_cameraZ += forwardZ * m_cameraSpeed;
        }
        if (m_keySPressed) {
            m_cameraX -= forwardX * m_cameraSpeed;
            m_cameraZ -= forwardZ * m_cameraSpeed;
        }
        if (m_keyAPressed) {
            m_cameraX += leftX * m_cameraSpeed;
            m_cameraZ -= leftZ * m_cameraSpeed;
        }
        if (m_keyDPressed) {
            m_cameraX -= leftX * m_cameraSpeed;
            m_cameraZ += leftZ * m_cameraSpeed;
        }
    }
    if (m_keyEPressed) {
        m_cameraY -= m_cameraSpeed;
    }
    if (m_keyQPressed) {
        m_cameraY += m_cameraSpeed;
    }
    if (m_keyUpPressed) {
        m_cameraPitch += m_rotationSpeed;
        if (m_cameraPitch > 90.0) m_cameraPitch = 90.0;
    }
    if (m_keyDownPressed) {
        m_cameraPitch -= m_rotationSpeed;
        if (m_cameraPitch < -90.0) m_cameraPitch = -90.0;
    }
    if (m_keyLeftPressed) {
        m_cameraYaw -= m_rotationSpeed;
    }
    if (m_keyRightPressed) {
        m_cameraYaw += m_rotationSpeed;
    }
    update();
}