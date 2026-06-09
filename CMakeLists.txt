cmake_minimum_required(VERSION 3.16)
project(GravitySimulation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Widgets
    Core
    Gui
    OpenGL
    OpenGLWidgets
)

set(SOURCES
    main.cpp
    GravitySimulation.cpp
    GravitySimulation.h
    SimulationGLWidget.cpp
    SimulationGLWidget.h
    PlanetInfoDialog.cpp
    PlanetInfoDialog.h
    AddPlanetDialog.cpp
    AddPlanetDialog.h
)

add_executable(GravitySimulation WIN32 ${SOURCES})

target_include_directories(GravitySimulation PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(GravitySimulation PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    opengl32
    glu32
)
