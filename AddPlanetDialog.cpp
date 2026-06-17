/**
 * @file AddPlanetDialog.cpp
 * @brief Реализация диалогового окна для добавления новой планеты в симуляцию.
 *
 * Данный файл содержит реализацию класса AddPlanetDialog, наследующегося от QDialog.
 * Диалог позволяет пользователю задать все необходимые параметры нового небесного тела:
 * - Имя (с проверкой на уникальность и отсутствие пустых значений).
 * - Массу, радиус и цвет объекта.
 * - Начальные координаты и вектор скорости.
 *
 * Окно является модальным, что предотвращает взаимодействие с основным интерфейсом
 * до завершения ввода данных или отмены операции. Кнопка "OK" активируется только
 * при корректном вводе имени. Выбор цвета осуществляется через стандартный диалог
 * QColorDialog.
 *
 * @author Шестаков Денис Сергеевич
 */

#include "AddPlanetDialog.h"

/**
 * @brief Конструктор класса AddPlanetDialog.
 *
 * Инициализирует модальное диалоговое окно "Добавить планету". Создает и настраивает
 * все элементы интерфейса: поля ввода, спинбоксы, кнопку выбора цвета и кнопки управления.
 * Настраивает диапазоны значений, шаги изменения и начальные значения параметров.
 * Подключает сигналы и слоты для валидации имени и выбора цвета.
 *
 * @param existingNames Список имен уже существующих планет, используемый для проверки
 *                      уникальности имени новой планеты.
 * @param parent Указатель на родительский виджет (по умолчанию nullptr).
 */
AddPlanetDialog::AddPlanetDialog(const QStringList& existingNames, QWidget* parent) : QDialog(parent), m_existingNames(existingNames)
{
    setWindowTitle("Добавить планету");
    setModal(true);

    auto* mainLayout = new QVBoxLayout(this);

    mainLayout->addWidget(new QLabel("Название:"));
    m_nameEdit = new QLineEdit(this);
    connect(m_nameEdit, &QLineEdit::textChanged, this, &AddPlanetDialog::onNameTextChanged);
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
    for (auto* spin : {m_posXSpin, m_posYSpin, m_posZSpin})
    {
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
    for (auto* spin : {m_velXSpin, m_velYSpin, m_velZSpin})
    {
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
    m_okButton = new QPushButton("OK", this);
    m_okButton->setEnabled(false);
    auto* cancelButton = new QPushButton("Отмена", this);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_okButton);
    buttonLayout->addWidget(cancelButton);

    connect(m_okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    mainLayout->addLayout(buttonLayout);

    // onNameTextChanged(m_nameEdit->text());
}

/**
 * @brief Обработчик нажатия кнопки "Выбрать цвет".
 *
 * Открывает стандартный системный диалог выбора цвета (QColorDialog).
 * Если пользователь выбрал цвет, обновляет внутреннее состояние (m_selectedColor)
 * и меняет стиль кнопки, отображая выбранный цвет в качестве фона.
 */
void AddPlanetDialog::onColorButtonClicked()
{
    QColor color = QColorDialog::getColor(m_selectedColor, this, "Выберите цвет планеты");
    if (color.isValid())
    {
        m_selectedColor = color;
        m_colorButton->setStyleSheet(QString("background-color: %1").arg(color.name()));
    }
}

/**
 * @brief Возвращает имя новой планеты, введенное пользователем.
 *
 * Используется главным окном после закрытия диалога с результатом Accepted (OK)
 * для создания объекта с указанным именем.
 *
 * @return QString Строка с именем планеты.
 */
QString AddPlanetDialog::getPlanetName() const
{
    return m_nameEdit->text();
}

/**
 * @brief Возвращает значение массы планеты, установленное в спинбоксе.
 *
 * Масса используется при создании объекта структуры Object.
 *
 * @return double Значение массы в условных единицах.
 */
double AddPlanetDialog::getMass() const
{
    return m_massSpin->value();
}

/**
 * @brief Возвращает значение радиуса планеты, установленное в спинбоксе.
 *
 * Радиус используется для отрисовки сферы планеты.
 *
 * @return double Значение радиуса в единицах мира.
 */
double AddPlanetDialog::getRadius() const
{
    return m_radiusSpin->value();
}

/**
 * @brief Возвращает вектор координат (X, Y, Z) новой планеты.
 *
 * Собирает значения из трех спинбоксов в один вектор std::vector<double>.
 *
 * @return std::vector<double> Вектор из трех элементов: {x, y, z}.
 */
std::vector<double> AddPlanetDialog::getPosition() const
{
    return {m_posXSpin->value(), m_posYSpin->value(), m_posZSpin->value()};
}

/**
 * @brief Возвращает вектор скорости (X, Y, Z) новой планеты.
 *
 * Собирает значения из трех спинбоксов в один вектор std::vector<double>.
 * Начальный вектор скорости определяет траекторию движения объекта.
 *
 * @return std::vector<double> Вектор из трех элементов: {vx, vy, vz}.
 */
std::vector<double> AddPlanetDialog::getVelocity() const
{
    return {m_velXSpin->value(), m_velYSpin->value(), m_velZSpin->value()};
}

/**
 * @brief Возвращает цвет планеты в формате RGB (нормализованные значения от 0.0 до 1.0).
 *
 * Преобразует объект QColor во внутренний формат симуляции (вектор из трех double).
 * Используются методы redF(), greenF(), blueF() для получения нормализованных значений.
 *
 * @return std::vector<double> Вектор из трех элементов: {r, g, b}.
 */
std::vector<double> AddPlanetDialog::getColor() const
{
    return {m_selectedColor.redF(), m_selectedColor.greenF(), m_selectedColor.blueF()};
}

/**
 * @brief Слот для валидации введенного имени планеты.
 *
 * Вызывается каждый раз при изменении текста в поле ввода имени.
 * Проверяет два условия:
 * 1. Имя не должно быть пустым.
 * 2. Имя не должно совпадать с именами уже существующих планет (регистронезависимое сравнение).
 * В зависимости от результата проверки включает или отключает кнопку "OK".
 *
 * @param text Текущий текст, введенный в поле имени.
 */
void AddPlanetDialog::onNameTextChanged(const QString& text)
{
    bool isValid = true;
    QString errorMessage;

    if (text.isEmpty())
    {
        isValid = false;
        errorMessage = "Имя не может быть пустым";
    }
    else if (m_existingNames.contains(text, Qt::CaseInsensitive))
    {
        isValid = false;
        errorMessage = "Планета с таким именем уже существует";
    }

    m_okButton->setEnabled(isValid);
}