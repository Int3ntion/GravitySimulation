/**
 * @file PlanetInfoDialog.cpp
 * @brief Реализация диалогового окна для отображения и редактирования информации о планете.
 *
 * Данный файл содержит реализацию класса PlanetInfoDialog, который наследуется от QDialog.
 * Окно предназначено для просмотра детальной информации об объекте симуляции:
 * - Название (доступно для редактирования).
 * - Масса (только для чтения).
 * - Текущие координаты (X, Y, Z).
 * - Текущая скорость (X, Y, Z).
 *
 * Также предоставляет функционал удаления планеты из симуляции через специальную кнопку.
 * Все поля, кроме имени, заблокированы для изменения пользователем, так как они управляются
 * физическим движком симуляции.
 *
 * @note Диалог является модальным (setModal(true)), что блокирует взаимодействие с главным
 * окном до закрытия диалога.
 *
 * @author Шестаков Денис Сергеевич
 */

#include "PlanetInfoDialog.h"

/**
 * @brief Конструктор класса PlanetInfoDialog.
 * 
 * Инициализирует модальное диалоговое окно с информацией о переданной планете.
 * Создает и настраивает все элементы интерфейса: метки, поля ввода, спинбоксы
 * и кнопки управления. Настраивает компоновку (layout) элементов.
 * 
 * @param planet Константная ссылка на объект планеты (структура Object),
 *               данные которой будут отображены в диалоге.
 * @param parent Указатель на родительский виджет (по умолчанию nullptr).
 */
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

/**
 * @brief Обработчик нажатия кнопки "Удалить планету".
 *
 * Вызывается при нажатии на кнопку удаления. Устанавливает специальный код возврата (-1),
 * который сигнализирует главному окну о необходимости удалить выбранную планету из вектора объектов.
 * После этого диалог закрывается.
 */
void PlanetInfoDialog::onDeleteButtonClicked() {
    done(-1);
}

/**
 * @brief Возвращает новое имя планеты, введенное пользователем в поле редактирования.
 *
 * Используется главным окном после успешного закрытия диалога (accept), чтобы обновить
 * имя объекта в симуляции. Если пользователь ничего не изменил, возвращается исходное имя.
 *
 * @return QString Строка с новым именем планеты.
 */
QString PlanetInfoDialog::getNewName() const {
    return m_nameEdit->text();
}