/**
 * @file SimulationGLWidget.cpp
 * @brief Реализация виджета OpenGL для симуляции гравитационного взаимодействия
 * и отрисовки 3D-сцены.
 *
 * Данный файл содержит реализацию класса SimulationGLWidget, который
 * наследуется от QOpenGLWidget. Он отвечает за:
 * - Инициализацию контекста OpenGL и настройку проекции.
 * - Расчет гравитационных сил между объектами (N-body simulation).
 * - Построение карты гравитационного потенциала (сетка).
 * - Управление камерой от первого лица.
 * - Отрисовку планет в виде сфер и гравитационной сетки.
 *
 * @author Шестаков Денис Сергеевич
 */

#include "SimulationGLWidget.h"

#include <Windows.h>
#include <gl/GLU.h>

#include <cmath>

/**
 * @class Object
 * @brief Вспомогательный класс (структура данных), описывающий физическое тело
 * в симуляции.
 *
 * Хранит параметры планеты/объекта: позицию, скорость, массу, радиус, цвет и
 * имя. Содержит метод отрисовки сферы.
 */

/**
 * @brief Отрисовывает объект в виде сферы с использованием фиксированного
 * конвейера OpenGL.
 *
 * Сфера строится методом меридианов и параллелей (quad strip).
 * Цвет устанавливается перед отрисовкой, позиция смещается относительно центра
 * мира.
 *
 */
void Object::drawSphere() const
{
    glColor3f(color[0], color[1], color[2]);
    const int slices = 64, stacks = 64;
    const float radius = static_cast<float>(this->radius);

    for (int i = 0; i < stacks; ++i)
    {
        float lat0 = M_PI * (-0.5f + static_cast<float>(i) / stacks);
        float z0 = std::sin(lat0);
        float r0 = std::cos(lat0);

        float lat1 = M_PI * (-0.5f + static_cast<float>(i + 1) / stacks);
        float z1 = std::sin(lat1);
        float r1 = std::cos(lat1);

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= slices; ++j)
        {
            float lng = 2.0f * M_PI * static_cast<float>(j) / slices;
            float x = std::cos(lng);
            float y = std::sin(lng);

            glVertex3f(static_cast<float>(position[0]) + radius * x * r0,
                       static_cast<float>(position[1]) + radius * y * r0,
                       static_cast<float>(position[2]) + radius * z0);
            glVertex3f(static_cast<float>(position[0]) + radius * x * r1,
                       static_cast<float>(position[1]) + radius * y * r1,
                       static_cast<float>(position[2]) + radius * z1);
        }
        glEnd();
    }
}

/**
 * @brief Конструктор класса SimulationGLWidget.
 *
 * Инициализирует виджет, настраивает политику фокуса, создает таймер для
 * обновления кадров и подключает сигналы таймера к слотам обновления физики и
 * перерисовки. Сразу после создания запускает симуляцию.
 *
 * @param parent Указатель на родительский виджет.
 */
SimulationGLWidget::SimulationGLWidget(QWidget* parent)
    : QOpenGLWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &SimulationGLWidget::updateObjects);
    connect(m_timer, &QTimer::timeout, this,
            static_cast<void (QOpenGLWidget::*)()>(&QOpenGLWidget::update));

    startSimulation();
}

/**
 * @brief Деструктор класса SimulationGLWidget.
 *
 * Освобождает ресурсы, занятые таймером.
 */
SimulationGLWidget::~SimulationGLWidget() { delete m_timer; }

/**
 * @brief Инициализация контекста OpenGL при первом создании виджета.
 *
 * Включает тест глубины (GL_DEPTH_TEST) для корректного перекрытия объектов.
 * Заполняет начальный вектор объектов (Солнце, Земля, Метеор) и рассчитывает
 * начальное гравитационное поле. Генерирует сигнал об изменении списка планет.
 */
void SimulationGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);

    // Инициализация объектов
    m_objects = {
        Object({0.0, 500.0, 0.0}, {0.0, 0.0, 0.0}, 1.98e30, 300.0,
               {1.0f, 0.4f, 0.0f}, "Солнце"),
        Object({-1000.0, 0.0, 0.0}, {0.0, -320000000.0, 5000000.0}, 1.972e8, 20.0,
               {0.0f, 0.4f, 0.5f}, "Метеор"),
        // Object({500.0, 0.0, 700.0}, {0.0, -4000000.0,
        // -3000000.0}, 1.972e23, 50.0, {0.8f, 0.04f, 0.04f}, "Меркурий"),
        Object({-2000.0, 500.0, 0.0}, {0.0, 0.0, 220000000.0}, 5.972e24, 50.0,
               {0.1f, 0.2f, 0.5f}, "Земля")};
    emit planetListChanged();
    calculateGravityField();
}

/**
 * @brief Обработка изменения размера окна OpenGL-контекста.
 *
 * Настраивает область просмотра (viewport) и устанавливает перспективную
 * проекцию. Угол обзора устанавливается 45 градусов, соотношение сторон берется
 * из новых размеров окна. Дальняя плоскость отсечения установлена на 40000
 * единиц.
 *
 * @param w Новая ширина окна в пикселях.
 * @param h Новая высота окна в пикселях.
 */
void SimulationGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, static_cast<float>(w) / h, 0.1f, 40000.0f);
}

/**
 * @brief Основной метод отрисовки кадра (вызывается каждый раз при обновлении
 * виджета).
 *
 * Выполняет следующие действия:
 * 1. Очищает буфер цвета и глубины.
 * 2. Настраивает матрицу вида (ModelView) с учетом положения и вращения камеры.
 * 3. Отрисовывает сетку гравитационного поля (линии).
 * 4. Отрисовывает все объекты (планеты) как сферы.
 */
void SimulationGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(static_cast<GLfloat>(m_cameraPitch), 1.0f, 0.0f, 0.0f);
    glRotatef(static_cast<GLfloat>(m_cameraYaw), 0.0f, 1.0f, 0.0f);

    glTranslatef(static_cast<GLfloat>(m_cameraX), static_cast<GLfloat>(m_cameraY),
                 static_cast<GLfloat>(m_cameraZ));

    glColor3f(0.2f, 0.2f, 0.2f);              // Серый цвет для гравитационной сетки
    const int gridSize = 5000;                // Размер сетки (в единицах мира)
    const int resolution = m_gridResolution;  // Разрешение сетки
    const double step = (2.0 * gridSize) / resolution;
    glBegin(GL_LINES);

    for (int i = 0; i <= resolution; ++i)
    {
        for (int j = 0; j < resolution; ++j)
        {
            double x1 = -gridSize + i * step;
            double z1 = -gridSize + j * step;
            double y1 = m_gravityField[i][j];

            double x2 = -gridSize + i * step;
            double z2 = -gridSize + (j + 1) * step;
            double y2 = m_gravityField[i][j + 1];

            glVertex3f(static_cast<float>(x1), static_cast<float>(y1),
                       static_cast<float>(z1));
            glVertex3f(static_cast<float>(x2), static_cast<float>(y2),
                       static_cast<float>(z2));
        }
    }

    for (int j = 0; j <= resolution; ++j)
    {
        for (int i = 0; i < resolution; ++i)
        {
            double x1 = -gridSize + i * step;
            double z1 = -gridSize + j * step;
            double y1 = m_gravityField[i][j];

            double x2 = -gridSize + (i + 1) * step;
            double z2 = -gridSize + j * step;
            double y2 = m_gravityField[i + 1][j];

            glVertex3f(static_cast<float>(x1), static_cast<float>(y1),
                       static_cast<float>(z1));
            glVertex3f(static_cast<float>(x2), static_cast<float>(y2),
                       static_cast<float>(z2));
        }
    }
    glEnd();

    for (const auto& obj : m_objects)
    {
        obj.drawSphere();
    }
}

/**
 * @brief Рассчитывает суммарную гравитационную силу, действующую на каждый
 * объект.
 *
 * Реализует упрощенную модель N-body задачи. Для каждой пары объектов
 * вычисляется сила притяжения по закону всемирного тяготения. Вводится
 * ограничение минимального расстояния (150 единиц) для предотвращения численной
 * нестабильности при столкновении. Ускорение применяется непосредственно к
 * скорости объекта через метод accelerate.
 */
void SimulationGLWidget::calculateForces()
{
    for (auto& obj : m_objects)
    {
        for (const auto& obj2 : m_objects)
        {
            if (&obj2 == &obj)
                continue;

            double dx = obj2.position[0] - obj.position[0];
            double dy = obj2.position[1] - obj.position[1];
            double dz = obj2.position[2] - obj.position[2];
            double distance = std::sqrt(dx * dx + dy * dy + dz * dz);
            if (distance < 150)
                distance = 150;

            std::vector<double> direction = {dx / distance, dy / distance,
                                             dz / distance};
            double acc1 = G * obj2.mass / (distance * distance);
            std::vector<double> acc = {acc1 * direction[0], acc1 * direction[1],
                                       acc1 * direction[2]};
            obj.accelerate(acc[0] * dt, acc[1] * dt, acc[2] * dt);
        }
    }
}

/**
 * @brief Обновляет состояние симуляции (физика + камера).
 *
 * Вызывается по таймеру. Последовательность действий:
 * 1. Расчет сил и обновление скоростей объектов.
 * 2. Обновление позиций объектов.
 * 3. Пересчет карты гравитационного поля.
 * 4. Обновление позиции камеры на основе нажатых клавиш.
 * 5. Запрос перерисовки виджета.
 */
void SimulationGLWidget::updateObjects()
{
    calculateForces();
    for (auto& obj : m_objects)
    {
        obj.updatePos(dt);
    }
    calculateGravityField();
    updateCamera();
    update();
}

/**
 * @brief Запускает симуляцию, устанавливая шаг времени и запуская таймер.
 *
 * Шаг времени dt устанавливается равным 1e-7 (очень маленький шаг для
 * стабильности). Таймер запускается с интервалом 16 мс, что соответствует
 * примерно 60 FPS.
 */
void SimulationGLWidget::startSimulation()
{
    dt = 1e-7;
    m_timer->start(16);  // 1000/FPS
}

/**
 * @brief Останавливает симуляцию.
 *
 * Если пробел не нажат, просто останавливает таймер.
 * Если пробел нажат (режим паузы), устанавливает шаг времени в 0, чтобы физика
 * не продолжала считаться даже если таймер вдруг сработает.
 */
void SimulationGLWidget::stopSimulation()
{
    if (m_keySpacePressed)
    {
        dt = 0;
    }
    else
    {
        m_timer->stop();
    }
}

/**
 * @brief Рассчитывает карту гравитационного потенциала для визуализации сетки.
 *
 * Создает двумерный массив m_gravityField размером
 * (resolution+1)x(resolution+1). Для каждой точки сетки суммируется потенциал
 * от всех объектов в симуляции. Формула потенциала упрощена для визуализации: V
 * = -A * r / sqrt(dx^2 + dy^2 + r^2), где A зависит от логарифма массы.
 * Результат смещается на -300 для лучшей видимости.
 */
void SimulationGLWidget::calculateGravityField()
{
    const int size = m_gridResolution;
    const double worldSize = 4000.0;
    const double step = (2.0 * worldSize) / size;

    m_gravityField = std::vector<std::vector<double>>(
        size + 1, std::vector<double>(size + 1, 0.0));

    for (int i = 0; i <= size; ++i)
    {
        for (int j = 0; j <= size; ++j)
        {
            double x = -worldSize + i * step;
            double z = -worldSize + j * step;
            double totalPotential = 0.0;

            for (const auto& obj : m_objects)
            {
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

/**
 * @brief Обработчик нажатия клавиш клавиатуры.
 *
 * Регистрирует нажатие клавиш и записывает в соответствующие флаги-члены
 * класса. Поддерживает управление камерой (WASD, QE, стрелки) и паузу (Space).
 * Необработанные клавиши передаются базовому классу.
 *
 * @param event Указатель на событие нажатия клавиши (QKeyEvent).
 */
void SimulationGLWidget::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
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
            m_keyDownPressed = true;
            break;
        case Qt::Key_Up:
            m_keyUpPressed = true;
            break;
        case Qt::Key_Left:
            m_keyLeftPressed = true;
            break;
        case Qt::Key_Right:
            m_keyRightPressed = true;
            break;
        case Qt::Key_Space:
            m_keySpacePressed = true;
        default:
            QOpenGLWidget::keyPressEvent(event);
    }
}

/**
 * @brief Обработчик отпускания клавиш клавиатуры.
 *
 * Сбрасывает флаги состояния нажатых клавиш в значение false, когда
 * пользователь отпускает соответствующую кнопку. Это позволяет прекратить
 * движение камеры или другие действия, привязанные к удержанию клавиши.
 * Необработанные клавиши передаются базовому классу для стандартной обработки.
 *
 * @param event Указатель на событие отпускания клавиши (QKeyEvent).
 */
void SimulationGLWidget::keyReleaseEvent(QKeyEvent* event)
{
    switch (event->key())
    {
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
            m_keyDownPressed = false;
            break;
        case Qt::Key_Up:
            m_keyUpPressed = false;
            break;
        case Qt::Key_Left:
            m_keyLeftPressed = false;
            break;
        case Qt::Key_Right:
            m_keyRightPressed = false;
            break;
        case Qt::Key_Space:
            m_keySpacePressed = false;
        default:
            QOpenGLWidget::keyReleaseEvent(event);
    }
}

/**
 * @brief Обновляет позицию и ориентацию камеры на основе состояния клавиш.
 *
 * Метод реализует управление камерой от первого лица:
 * - **Перемещение (W, A, S, D):** Движение вперед/назад и стрейф влево/вправо
 *   относительно текущего направления взгляда (учитывается угол рысканья
 * m_cameraYaw).
 * - **Вертикальное перемещение (Q, E):** Подъем и спуск по оси Y.
 * - **Вращение (Стрелки):** Изменение угла тангажа (Pitch) и рысканья (Yaw).
 *   Угол тангажа ограничен диапазоном [-90, 90] для предотвращения инверсии
 * камеры.
 * - **Пауза симуляции (Space):** Если пробел удерживается, симуляция
 * останавливается, если отпущен — запускается снова.
 *
 * После обновления параметров камеры запрашивается перерисовка виджета.
 */
void SimulationGLWidget::updateCamera()
{
    if (m_keyWPressed || m_keySPressed || m_keyAPressed || m_keyDPressed)
    {
        double forwardX = std::sin(-m_cameraYaw * M_PI / 180.0);
        double forwardZ = std::cos(m_cameraYaw * M_PI / 180.0);
        double leftX = std::sin((m_cameraYaw + 90.0) * M_PI / 180.0);
        double leftZ = std::cos((m_cameraYaw + 90.0) * M_PI / 180.0);

        if (m_keyWPressed)
        {
            m_cameraX += forwardX * m_cameraSpeed;
            m_cameraZ += forwardZ * m_cameraSpeed;
        }
        if (m_keySPressed)
        {
            m_cameraX -= forwardX * m_cameraSpeed;
            m_cameraZ -= forwardZ * m_cameraSpeed;
        }
        if (m_keyAPressed)
        {
            m_cameraX += leftX * m_cameraSpeed;
            m_cameraZ -= leftZ * m_cameraSpeed;
        }
        if (m_keyDPressed)
        {
            m_cameraX -= leftX * m_cameraSpeed;
            m_cameraZ += leftZ * m_cameraSpeed;
        }
    }
    if (m_keyEPressed)
    {
        m_cameraY -= m_cameraSpeed;
    }
    if (m_keyQPressed)
    {
        m_cameraY += m_cameraSpeed;
    }
    if (m_keyUpPressed)
    {
        m_cameraPitch -= m_rotationSpeed;
        if (m_cameraPitch > 90.0)
            m_cameraPitch = 90.0;
    }
    if (m_keyDownPressed)
    {
        m_cameraPitch += m_rotationSpeed;
        if (m_cameraPitch < -90.0)
            m_cameraPitch = -90.0;
    }
    if (m_keyLeftPressed)
    {
        m_cameraYaw -= m_rotationSpeed;
    }
    if (m_keyRightPressed)
    {
        m_cameraYaw += m_rotationSpeed;
    }
    if (m_keySpacePressed)
    {
        stopSimulation();
    }
    if (!m_keySpacePressed)
    {
        startSimulation();
    }
    update();
}