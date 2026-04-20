#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_GravitySimulation.h"

class GravitySimulation : public QMainWindow
{
    Q_OBJECT

public:
    GravitySimulation(QWidget *parent = nullptr);
    ~GravitySimulation();

private:
    Ui::GravitySimulationClass ui;
};

