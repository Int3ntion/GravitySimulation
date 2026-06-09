#include "PlanetInfoDialog.h"

PlanetInfoDialog::PlanetInfoDialog(const Object& planet, QWidget* parent) : QDialog(parent) {
	setWindowTitle("Информация о планете " + planet.name);
	setModal(true);

	auto* mainLayout = new QVBoxLayout(this);

	mainLayout->addWidget(new QLabel("Название:"));
	m_nameEdit = new QLineEdit(planet.name, this);
	mainLayout->addWidget(m_nameEdit);

	mainLayout->addWidget(new QLabel("Масса:"));
	m_massSpin = new QDoubleSpinBox(this);
	m_massSpin->setValue(planet.mass);
	m_massSpin->setReadOnly(true);
	m_massSpin->setButtonSymbols(QAbstractSpinBox::NoButtons);
	mainLayout->addWidget(m_massSpin);

    mainLayout->addWidget(new QLabel("Координаты (X, Y, Z):"));
    auto* posLayout = new QHBoxLayout();
    m_posXSpin = new QDoubleSpinBox(this);
    m_posYSpin = new QDoubleSpinBox(this);
    m_posZSpin = new QDoubleSpinBox(this);
    for (auto* spin : { m_posXSpin, m_posYSpin, m_posZSpin }) {
        spin->setReadOnly(true);
        spin->setRange(-1e10, 1e10);
        spin->setButtonSymbols(QAbstractSpinBox::NoButtons);
        posLayout->addWidget(spin);
    }
    m_posXSpin->setValue(planet.position[0]);
    m_posYSpin->setValue(planet.position[1]);
    m_posZSpin->setValue(planet.position[2]);
    mainLayout->addLayout(posLayout);

    mainLayout->addWidget(new QLabel("Скорость (X, Y, Z):"));
    auto* velLayout = new QHBoxLayout();
    m_velXSpin = new QDoubleSpinBox(this);
    m_velYSpin = new QDoubleSpinBox(this);
    m_velZSpin = new QDoubleSpinBox(this);
    for (auto* spin : { m_velXSpin, m_velYSpin, m_velZSpin }) {
        spin->setReadOnly(true);
        spin->setRange(-1e10, 1e10);
        spin->setButtonSymbols(QAbstractSpinBox::NoButtons);
        velLayout->addWidget(spin);
    }
    m_velXSpin->setValue(planet.velocity[0]);
    m_velYSpin->setValue(planet.velocity[1]);
    m_velZSpin->setValue(planet.velocity[2]);
    mainLayout->addLayout(velLayout);

    auto* buttonLayout = new QHBoxLayout();
    auto* okButton = new QPushButton("OK", this);
    m_deleteButton = new QPushButton("Удалить планету", this);
    auto* cancelButton = new QPushButton("Отмена", this);

    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(m_deleteButton);
    buttonLayout->addWidget(cancelButton);

    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_deleteButton, &QPushButton::clicked, this, &PlanetInfoDialog::onDeleteButtonClicked);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    mainLayout->addLayout(buttonLayout);
}

void PlanetInfoDialog::onDeleteButtonClicked() {
    done(-1);
}

QString PlanetInfoDialog::getNewName() const {
    return m_nameEdit->text();
}