cmake_minimum_required(VERSION 4.2)
project(GravitySimulation LANGUAGES CXX)
enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include(FetchContent)
FetchContent_Declare(
  doctest
  GIT_REPOSITORY https://github.com/doctest/doctest.git
  GIT_TAG v2.4.11
)
FetchContent_MakeAvailable(doctest)

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
    SimulationExceptions.h
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

add_executable(tests_main 
    tests/test_main.cpp
    SimulationGLWidget.cpp
    AddPlanetDialog.cpp
    GravitySimulation.cpp
    PlanetInfoDialog.cpp
)

target_link_libraries(tests_main PRIVATE 
    doctest::doctest
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    opengl32
    glu32
)

target_include_directories(tests_main PRIVATE 
    ${CMAKE_SOURCE_DIR}
)

add_test(
    NAME Tests_main
    COMMAND tests_main.exe
)

add_executable(tests_exceptions 
    tests/test_exceptions.cpp
    SimulationGLWidget.cpp
    AddPlanetDialog.cpp
    GravitySimulation.cpp
    PlanetInfoDialog.cpp
)

target_link_libraries(tests_exceptions PRIVATE 
    doctest::doctest
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    opengl32
    glu32
)

target_include_directories(tests_exceptions PRIVATE 
    ${CMAKE_SOURCE_DIR}
)

add_test(
    NAME Tests_exceptions
    COMMAND tests_exceptions.exe
)

