#pragma once

#include <QtWidgets/QMainWindow>
#include <QOpenGLWidget>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>
#include <iostream>
#include <QLabel>
#include "ui_GravitySimulation.h"
#include "SimulationGLWidget.h"

class GravitySimulation : public QMainWindow
{
    Q_OBJECT

public:
    GravitySimulation(QWidget *parent = nullptr);
    ~GravitySimulation();
    void updatePlanetList();

public slots:
    void onStartSimulation();
    void onStopSimulation();
    void onAddPlanet();
    void onPlanetDoubleClicked(QListWidgetItem* item);
    void onGSliderChanged(int value);

private:
    Ui::GravitySimulationClass ui;
    SimulationGLWidget* m_glWidget;
    QListWidget* m_listWidget;
    QHBoxLayout* m_mainLayout;
    QSlider* m_gSlider;
    QLabel* m_gLabel;

    void setupUI();
};
