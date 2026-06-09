#include "AddPlanetDialog.h"

AddPlanetDialog::AddPlanetDialog(QWidget* parent) : QDialog(parent) {
	setWindowTitle("Добавить планету");
	setModal(true);

	auto* mainLayout = new QVBoxLayout(this);

	mainLayout->addWidget(new QLabel("Название:"));
	m_nameEdit = new QLineEdit(this);
	mainLayout->addWidget(m_nameEdit);

	mainLayout->addWidget(new QLabel("Масса:"));
	m_massSpin = new QDoubleSpinBox(this);
	m_massSpin->setRange(1e10, 1e33);
	m_massSpin->setSingleStep(1e20);
	m_massSpin->setValue(5.972e24);
	mainLayout->addWidget(m_massSpin);

	mainLayout->addWidget(new QLabel("Координаты (X, Y, Z):"));
	auto* posLayout = new QHBoxLayout();
	m_posXSpin = new QDoubleSpinBox(this);
	m_posYSpin = new QDoubleSpinBox(this);
	m_posZSpin = new QDoubleSpinBox(this);
	for (auto* spin : { m_posXSpin, m_posYSpin, m_posZSpin }) {
		spin->setRange(-10000, 10000);
		spin->setSingleStep(100);
		posLayout->addWidget(spin);
	}
	mainLayout->addLayout(posLayout);

	mainLayout->addWidget(new QLabel("Скорость (по X, Y, Z):"));
	auto* velLayout = new QHBoxLayout();
	m_velXSpin = new QDoubleSpinBox(this);
	m_velYSpin = new QDoubleSpinBox(this);
	m_velZSpin = new QDoubleSpinBox(this);
	for (auto* spin : { m_velXSpin, m_velYSpin, m_velZSpin }) {
		spin->setRange(-1e8, 1e8);
		spin->setSingleStep(1e5);
		velLayout->addWidget(spin);
	}
	mainLayout->addLayout(velLayout);

	mainLayout->addWidget(new QLabel("Цвет:"));
	m_colorButton = new QPushButton("Выбрать цвет", this);
	connect(m_colorButton, &QPushButton::clicked, this, &AddPlanetDialog::onColorButtonClicked);
	mainLayout->addWidget(m_colorButton);

	mainLayout->addWidget(new QLabel("Радиус:"));
	m_radiusSpin = new QDoubleSpinBox(this);
	m_radiusSpin->setRange(10, 1000);
	m_radiusSpin->setSingleStep(5);
	m_radiusSpin->setValue(100);
	mainLayout->addWidget(m_radiusSpin);

	auto* buttonLayout = new QHBoxLayout();
	auto* okButton = new QPushButton("OK", this);
	auto* cancelButton = new QPushButton("Отмена", this);
	buttonLayout->addStretch();
	buttonLayout->addWidget(okButton);
	buttonLayout->addWidget(cancelButton);

	connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
	connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

	mainLayout->addLayout(buttonLayout);
}

void AddPlanetDialog::onColorButtonClicked() {
	QColor color = QColorDialog::getColor(m_selectedColor, this, "Выберите цвет планеты");
	if (color.isValid()) {
		m_selectedColor = color;
		m_colorButton->setStyleSheet(QString("background-color: %1").arg(color.name()));
	}
}

QString AddPlanetDialog::getPlanetName() const {
	return m_nameEdit->text();
}

double AddPlanetDialog::getMass() const {
	return m_massSpin->value();
}

double AddPlanetDialog::getRadius() const {
	return m_radiusSpin->value();
}

std::vector<double> AddPlanetDialog::getPosition() const {
	return { m_posXSpin->value(), m_posYSpin->value(), m_posZSpin->value() };
}

std::vector<double> AddPlanetDialog::getVelocity() const {
	return { m_velXSpin->value(), m_velYSpin->value(), m_velZSpin->value() };
}

std::vector<double> AddPlanetDialog::getColor() const {
	return { m_selectedColor.redF(), m_selectedColor.greenF(), m_selectedColor.blueF() };
}