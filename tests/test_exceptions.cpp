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

TEST_CASE("SimulationGLWidget: Constructor throws InitializationException")
{
    getApp();

    CHECK_THROWS_AS(
        []()
        {
            throw InitializationException("Тест конструктора");
        }(),
        InitializationException);
}

TEST_CASE("SimulationGLWidget: calculateGravityField throws SimulationLogicException")
{
    getApp();

    SimulationGLWidget widget;
    widget.m_objects.clear();

    widget.m_gridResolution = 0;

    CHECK_THROWS_AS(widget.calculateGravityField(), SimulationLogicException);
}

TEST_CASE("SimulationGLWidget: calculateGravityField throws on division by zero protection")
{
    getApp();

    SimulationGLWidget widget;
    widget.m_objects.clear();
    widget.m_gridResolution = 10;

    widget.m_gridResolution = -5;
    CHECK_THROWS_AS(widget.calculateGravityField(), SimulationLogicException);
}

TEST_CASE("Exceptions: Throwing InvalidPlanetDataException")
{
    getApp();

    try
    {
        // Эмуляция ситуации: пользователь ввел отрицательную массу
        throw InvalidPlanetDataException("Масса не может быть отрицательной!");

        // Если код дошел сюда, значит throw не сработал
        FAIL("Exception was not thrown");
    }
    catch (const InvalidPlanetDataException& e)
    {
        // Успех: мы поймали правильное исключение
        CHECK(QString::fromStdString(e.what()).contains("отрицательной"));
    }
    catch (...)
    {
        FAIL("Caught wrong exception type");
    }
}

TEST_CASE("SimulationGLWidget: updateObjects handles exceptions gracefully")
{
    getApp();

    SimulationGLWidget widget;
    widget.m_objects.clear();
    widget.m_gridResolution = 0;

    widget.m_objects.push_back(Object({0, 0, 0}, {0, 0, 0}, 1, 1, {}, "Planet"));

    widget.updateObjects();

    CHECK(!widget.m_timer->isActive());
}