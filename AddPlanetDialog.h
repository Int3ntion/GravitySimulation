#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QColorDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

class AddPlanetDialog : public QDialog {
	Q_OBJECT

public:
	explicit AddPlanetDialog(QWidget* parent = nullptr);
	QString getPlanetName() const;
	double getMass() const;
	double getRadius() const;
	std::vector<double> getPosition() const;
	std::vector<double> getVelocity() const;
	std::vector<double> getColor() const;

private slots:
	void onColorButtonClicked();

private:
	QLineEdit* m_nameEdit;
	QDoubleSpinBox* m_massSpin;
	QDoubleSpinBox* m_radiusSpin;
	QDoubleSpinBox* m_posXSpin, * m_posYSpin, * m_posZSpin;
	QDoubleSpinBox* m_velXSpin, * m_velYSpin, * m_velZSpin;
	QPushButton* m_colorButton;
	QColor m_selectedColor;
};