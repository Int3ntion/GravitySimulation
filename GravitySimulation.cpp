#include "GravitySimulation.h"
#include "SimulationGLWidget.h"

GravitySimulation::GravitySimulation(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    setupUI();
}

GravitySimulation::~GravitySimulation()
{}

void GravitySimulation::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    m_mainLayout = new QHBoxLayout(centralWidget);

    m_listWidget = new QListWidget(this);
    m_listWidget->setFixedWidth(300);
    m_listWidget->clear();

    m_glWidget = new SimulationGLWidget(this);
    m_mainLayout->addWidget(m_listWidget);
    m_mainLayout->addWidget(m_glWidget, 1);
    centralWidget->setLayout(m_mainLayout);
}

void GravitySimulation::onStartSimulation() {
    m_glWidget->startSimulation();
}

void GravitySimulation::onStopSimulation() {
    m_glWidget->stopSimulation();
}