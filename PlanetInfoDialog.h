#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <SimulationGLWidget.h>

class PlanetInfoDialog : public QDialog {
	Q_OBJECT

public:
	explicit PlanetInfoDialog(const Object& planet, QWidget* parent = nullptr);
	QString getNewName() const;

private slots:
	void onDeleteButtonClicked();

private:
	QLineEdit* m_nameEdit;
	QDoubleSpinBox* m_massSpin;
	QDoubleSpinBox* m_posXSpin, * m_posYSpin, * m_posZSpin;
	QDoubleSpinBox* m_velXSpin, * m_velYSpin, * m_velZSpin;
	QPushButton* m_deleteButton;
	int m_planetIndex;
};