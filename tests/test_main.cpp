#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_NO_MULTITHREADING

#include <doctest/doctest.h>

#include <QApplication>
#include <QStringList>
#include <cmath>
#include <mutex>
#include <stdexcept>

#include "../AddPlanetDialog.h"
#include "../GravitySimulation.h"
#include "../SimulationExceptions.h"
#include "../SimulationGLWidget.h"

static QApplication* app_ptr = nullptr;

QApplication& getApp()
{
    if (!app_ptr)
    {
        int fake_argc = 1;
        char* fake_argv[] = {(char*)"tests", nullptr};

        static QApplication app(fake_argc, fake_argv);
        app_ptr = &app;
    }
    return *app_ptr;
}

TEST_CASE("Object: Constructor")
{
    getApp();

    std::vector<double> pos = {0.0, 0.0, 0.0};
    std::vector<double> vel = {10.0, 0.0, 0.0};
    double mass = 1000.0;
    double radius = 50.0;
    QString name = "TestPlanet";

    Object obj(pos, vel, mass, radius, {1, 0, 0}, name);

    CHECK(obj.name == name);
    CHECK(obj.mass == mass);
    CHECK(obj.radius == radius);

    CHECK(obj.position[0] == 0.0);
    CHECK(obj.velocity[0] == 10.0);
}

TEST_CASE("Object: Velocity and position")
{
    getApp();

    std::vector<double> pos = {0.0, 0.0, 0.0};
    std::vector<double> vel = {0.0, 0.0, 0.0};

    Object obj(pos, vel, 1000.0, 50.0, {1, 1, 1}, "MovingObj");

    double ax = 1.0, ay = 0.5, az = -0.2;
    double dt = 1.0;

    obj.accelerate(ax * dt, ay * dt, az * dt);
    obj.updatePos(dt);

    CHECK(obj.velocity[0] == doctest::Approx(1.0));
    CHECK(obj.velocity[1] == doctest::Approx(0.5));
    CHECK(obj.velocity[2] == doctest::Approx(-0.2));

    CHECK(obj.position[0] == doctest::Approx(1.0));
    CHECK(obj.position[1] == doctest::Approx(0.5));
    CHECK(obj.position[2] == doctest::Approx(-0.2));
}

TEST_CASE("SimulationGLWidget: Gravity calculate")
{
    getApp();

    SimulationGLWidget widget;
    widget.m_objects.clear();
    widget.dt = 1.0;

    Object heavy({0, 0, 0}, {0, 0, 0}, 1e20, 100, {1, 0, 0}, "Heavy");
    Object light({1000, 0, 0}, {0, 0, 0}, 1e10, 10, {0, 1, 0}, "Light");

    widget.m_objects.push_back(heavy);
    widget.m_objects.push_back(light);

    widget.G = 6.6743e-11;

    widget.calculateForces();

    const auto& updatedLight = widget.m_objects[1];

    double expectedAcc = (widget.G * heavy.mass) / (1000.0 * 1000.0);
    double expectedVelChange = expectedAcc * widget.dt;

    CHECK(updatedLight.velocity[0] != 0.0);
    CHECK(std::abs(updatedLight.velocity[0]) == doctest::Approx(expectedVelChange));
}

TEST_CASE("SimulationGLWidget: Zero Distance Protection")
{
    getApp();

    SimulationGLWidget widget;
    widget.m_objects.clear();
    if (widget.dt == 0) widget.dt = 1.0;

    Object p1_data({0, 0, 0}, {0, 0, 0}, 1e20, 10, {}, "P1");
    Object p2_data({0, 0, 0}, {0, 0, 0}, 1e20, 10, {}, "P2");

    widget.m_objects.push_back(p1_data);
    widget.m_objects.push_back(p2_data);
    widget.G = 6.6743e-11;

    widget.calculateForces();

    const auto& obj1 = widget.m_objects[0];
    const auto& obj2 = widget.m_objects[1];

    CHECK(!std::isnan(obj1.velocity[0]));
    CHECK(!std::isinf(obj1.velocity[0]));

    CHECK(std::abs(obj1.velocity[0]) < 1e15);
    CHECK(std::abs(obj2.velocity[0]) < 1e15);
}

TEST_CASE("SimulationGLWidget: Gravity Formula Accuracy")
{
    getApp();

    SimulationGLWidget widget;
    widget.m_objects.clear();
    if (widget.dt == 0) widget.dt = 1.0;

    double distance = 1000.0;

    Object heavy({0, 0, 0}, {0, 0, 0}, 1e20, 100, {1, 0, 0}, "Heavy");
    Object light({distance, 0, 0}, {0, 0, 0}, 1e10, 10, {0, 1, 0}, "Light");

    widget.m_objects.push_back(heavy);
    widget.m_objects.push_back(light);
    widget.G = 6.6743e-11;

    widget.calculateForces();

    const auto& updatedLight = widget.m_objects[1];

    double expectedAcc = (widget.G * heavy.mass) / (distance * distance);
    double expectedVelocityChange = expectedAcc * widget.dt;

    CHECK(updatedLight.velocity[0] != 0.0);
    CHECK(std::abs(updatedLight.velocity[0]) == doctest::Approx(expectedVelocityChange));
    CHECK(updatedLight.velocity[1] == doctest::Approx(0.0));
    CHECK(updatedLight.velocity[2] == doctest::Approx(0.0));
}

TEST_CASE("AddPlanetDialog: Mass and radius")
{
    getApp();

    QStringList empty;
    AddPlanetDialog dialog(empty);

    dialog.m_massSpin->setValue(1e10);
    CHECK(dialog.getMass() == doctest::Approx(1e10));

    dialog.m_massSpin->setValue(1e33);
    CHECK(dialog.getMass() == doctest::Approx(1e33));

    dialog.m_radiusSpin->setValue(10.0);
    CHECK(dialog.getRadius() == 10.0);

    dialog.m_radiusSpin->setValue(1000.0);
    CHECK(dialog.getRadius() == 1000.0);
}

TEST_CASE("AddPlanetDialog: Coords and Velocity")
{
    getApp();

    QStringList empty;
    AddPlanetDialog dialog(empty);

    dialog.m_posXSpin->setValue(-5000);
    dialog.m_posYSpin->setValue(200);
    dialog.m_posZSpin->setValue(9999);
    auto pos = dialog.getPosition();
    CHECK(pos[0] == -5000);
    CHECK(pos[1] == 200);
    CHECK(pos[2] == 9999);

    dialog.m_velXSpin->setValue(-1e7);
    dialog.m_velYSpin->setValue(5e6);
    dialog.m_velZSpin->setValue(0);
    auto vel = dialog.getVelocity();
    CHECK(vel[0] == doctest::Approx(-1e7));
    CHECK(vel[1] == doctest::Approx(5e6));
    CHECK(vel[2] == 0);
}

TEST_CASE("GravitySimulation: Planet list update")
{
    getApp();

    SimulationGLWidget glWidget;
    glWidget.m_objects.clear();

    glWidget.m_objects.push_back(Object({0, 0, 0}, {0, 0, 0}, 0, 0, {1, 1, 1}, "Sun"));
    glWidget.m_objects.push_back(Object({0, 0, 0}, {0, 0, 0}, 0, 0, {1, 1, 1}, "Earth"));

    GravitySimulation sim;
    CHECK(glWidget.m_objects.size() == 2);
    CHECK(glWidget.m_objects[0].name == "Sun");
    CHECK(glWidget.m_objects[1].name == "Earth");
}

TEST_CASE("Physics: Conservation of Momentum Approximation Negative Case")
{
    getApp();

    SimulationGLWidget widget;
    widget.m_objects.clear();
    widget.G = 0;

    Object p1({-100, 0, 0}, {10, 0, 0}, 100.0, 10, {1, 1, 1}, "Left");
    Object p2({100, 0, 0}, {-10, 0, 0}, 100.0, 10, {1, 1, 1}, "Right");

    widget.m_objects.push_back(p1);
    widget.m_objects.push_back(p2);

    widget.updateObjects();

    double p1_new_momentum = widget.m_objects[0].mass * widget.m_objects[0].velocity[0];
    double p2_new_momentum = widget.m_objects[1].mass * widget.m_objects[1].velocity[0];

    double total_momentum = p1_new_momentum + p2_new_momentum;

    CHECK(total_momentum == doctest::Approx(0.0).epsilon(1e-9));  // epsilon - погрешность
}