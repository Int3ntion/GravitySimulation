#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QTimer>
#include <QKeyEvent>
#include <vector>

struct Object {
    std::vector<double> position;
    std::vector<double> velocity;
    double radius;
    double mass;
    std::vector<double> color;
    QString name;

    Object(std::vector<double> pos, std::vector<double> vel, double m, double r = 20.0f,
        std::vector<double> c = { 1.0f, 0.0f, 0.0f }, QString n = "")
        : position(pos), velocity(vel), radius(r), mass(m), color(c), name(n) {
    }

    void accelerate(double x, double y, double z) {
        velocity[0] += x;
        velocity[1] += y;
        velocity[2] += z;
    }

    void updatePos(double dt) {
        position[0] += velocity[0] * dt;
        position[1] += velocity[1] * dt;
        position[2] += velocity[2] * dt;
    }

    void drawSphere() const;
};

class SimulationGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    explicit SimulationGLWidget(QWidget* parent = nullptr);
    ~SimulationGLWidget();
    std::vector<Object> m_objects;

public slots:
    void startSimulation();
    void stopSimulation();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    QTimer* m_timer;
    double m_cameraX = 0.0;
    double m_cameraY = -1000.0;
    double m_cameraZ = -4500.0;
    double m_cameraSpeed = 100.0;
    double m_rotationSpeed = 1.0;
    double m_cameraPitch = 10.0;
    double m_cameraYaw = 0.0;

    bool m_keyWPressed = false;
    bool m_keyAPressed = false;
    bool m_keySPressed = false;
    bool m_keyDPressed = false;
    bool m_keyEPressed = false;
    bool m_keyQPressed = false;
    bool m_keyUpPressed = false;
    bool m_keyDownPressed = false;
    bool m_keyLeftPressed = false;
    bool m_keyRightPressed = false;

    std::vector<std::vector<double>> m_gravityField;
    int m_gridResolution = 100; // Разрешение сетки

    void calculateForces();
    void updateObjects();
    void calculateGravityField();
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);
    void updateCamera();
    static constexpr double G = 6.6743e-11;
    static constexpr double dt = 0.0000001;

};