#pragma once

#include <QtWidgets/QMainWindow>
#include <QOpenGLWidget>
#include <QListWidget>
#include <QHBoxLayout>
#include "ui_GravitySimulation.h"
#include "SimulationGLWidget.h"

class GravitySimulation : public QMainWindow
{
    Q_OBJECT

public:
    GravitySimulation(QWidget *parent = nullptr);
    ~GravitySimulation();

public slots:
    void onStartSimulation();
    void onStopSimulation();

private:
    Ui::GravitySimulationClass ui;
    SimulationGLWidget* m_glWidget;
    QListWidget* m_listWidget;
    QHBoxLayout* m_mainLayout;

    void setupUI();
};
