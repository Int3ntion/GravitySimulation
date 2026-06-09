#include "GravitySimulation.h"
#include "SimulationGLWidget.h"
#include "AddPlanetDialog.h"
#include "PlanetInfoDialog.h"
#include <QDebug>

GravitySimulation::GravitySimulation(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();

    connect(m_glWidget, &SimulationGLWidget::planetListChanged, this, &GravitySimulation::updatePlanetList);
}

GravitySimulation::~GravitySimulation()
{}

void GravitySimulation::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    m_mainLayout = new QHBoxLayout(centralWidget);

    QPushButton* btnAddPlanet = new QPushButton("Добавить планету", this);
    btnAddPlanet->setFixedHeight(50);
    connect(btnAddPlanet, &QPushButton::clicked, this, &GravitySimulation::onAddPlanet);

    QVBoxLayout* vBoxLayout = new QVBoxLayout();
    m_glWidget = new SimulationGLWidget(this);


    m_listWidget = new QListWidget(this);
    connect(m_listWidget, &QListWidget::itemDoubleClicked, this, &GravitySimulation::onPlanetDoubleClicked);
    m_listWidget->setFixedWidth(300);

    auto* gLayout = new QHBoxLayout();
    m_gSlider = new QSlider(Qt::Horizontal, this);
    m_gSlider->setRange(1, 200);   // 1e-12 – 2e-10 (масштабируем)
    m_gSlider->setValue(67);         // начальное значение ~6.7e-11
    gLayout->addWidget(new QLabel("Гравитационная постоянная (G):", this));
    gLayout->addWidget(m_gSlider);

    vBoxLayout->addWidget(m_listWidget);
    vBoxLayout->addLayout(gLayout);
    vBoxLayout->addWidget(btnAddPlanet);

    connect(m_gSlider, &QSlider::valueChanged, this, &GravitySimulation::onGSliderChanged);

    m_glWidget->setMinimumWidth(300);
    m_glWidget->setMinimumHeight(150);
    m_mainLayout->addLayout(vBoxLayout);
    m_mainLayout->addWidget(m_glWidget, 1); // 1 - Растяжение этого виджета

    centralWidget->setLayout(m_mainLayout);
}

void GravitySimulation::onStartSimulation() {
    m_glWidget->startSimulation();
}

void GravitySimulation::onStopSimulation() {
    m_glWidget->stopSimulation();
}

void GravitySimulation::updatePlanetList() {
    m_listWidget->clear();

    for (const auto& obj : m_glWidget->m_objects) {
        m_listWidget->addItem(obj.name);
    }
}

void GravitySimulation::onAddPlanet() {
    AddPlanetDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QString name = dialog.getPlanetName();
        double mass = dialog.getMass();
        std::vector<double> position = dialog.getPosition();
        std::vector<double> velocity = dialog.getVelocity();
        double radius = dialog.getRadius();
        std::vector<double> color = dialog.getColor();

        Object newPlanet(position, velocity, mass, radius, color, name);
        m_glWidget->m_objects.push_back(newPlanet);
        m_glWidget->update();
        updatePlanetList();
    }
}

void GravitySimulation::onPlanetDoubleClicked(QListWidgetItem* item) {
    int index = m_listWidget->row(item);
    if (index < 0 || index > m_glWidget->m_objects.size()) return;

    Object& planet = m_glWidget->m_objects[index];

    PlanetInfoDialog dialog(planet, this);
    int result = dialog.exec();

    if (result == QDialog::Accepted) {
        QString newName = dialog.getNewName();
        if (!newName.isEmpty()) {
            planet.name = newName;
            updatePlanetList();
        }
    }
    else if (result == -1) {
        m_glWidget->m_objects.erase(m_glWidget->m_objects.begin() + index);
        m_glWidget->update();
        updatePlanetList();
    }
}

void GravitySimulation::onGSliderChanged(int value) {
    double G = value * 1e-12;
    m_glWidget->G = G;
}