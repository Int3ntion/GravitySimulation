/**
 * @file GravitySimulation.cpp
 * @brief Реализация главного окна приложения симуляции гравитации.
 *
 * Этот файл содержит реализацию класса GravitySimulation, который управляет
 * основным интерфейсом приложения: отрисовкой сцены (через SimulationGLWidget),
 * списком планет, диалогами добавления/редактирования и настройкой
 * гравитационной постоянной.
 *
 * @author Шестаков Денис Сергеевич
 */

#include "GravitySimulation.h"

#include <QDebug>
#include <QMessageBox>

#include "AddPlanetDialog.h"
#include "PlanetInfoDialog.h"
#include "SimulationExceptions.h"
#include "SimulationGLWidget.h"

/**
 * @brief Конструктор класса GravitySimulation.
 *
 * Инициализирует главное окно, настраивает пользовательский интерфейс (UI)
 * и устанавливает соединения сигналов и слотов между виджетами и логикой приложения.
 *
 * @param parent Указатель на родительский виджет (по умолчанию nullptr).
 */
GravitySimulation::GravitySimulation(QWidget* parent)
    : QMainWindow(parent)
{
    setupUI();

    connect(m_glWidget, &SimulationGLWidget::planetListChanged, this, &GravitySimulation::updatePlanetList);
}

/**
 * @brief Деструктор класса GravitySimulation.
 *
 * Освобождает ресурсы, занятые объектом.
 */
GravitySimulation::~GravitySimulation()
{
}

/**
 * @brief Настраивает пользовательский интерфейс главного окна.
 *
 * Создает и компонует все элементы интерфейса:
 * - Центральную область с виджетом OpenGL (SimulationGLWidget).
 * - Панель управления слева со списком планет, слайдером G и кнопкой добавления.
 * - Устанавливает связи между элементами управления и слотами обработки событий.
 */
void GravitySimulation::setupUI()
{
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
    m_gSlider->setRange(1, 200);  // 1e-12 – 2e-10 (масштабируем)
    m_gSlider->setValue(67);      // начальное значение ~6.7e-11
    gLayout->addWidget(new QLabel("Гравитационная постоянная (G):", this));
    gLayout->addWidget(m_gSlider);

    vBoxLayout->addWidget(m_listWidget);
    vBoxLayout->addLayout(gLayout);
    vBoxLayout->addWidget(btnAddPlanet);

    connect(m_gSlider, &QSlider::valueChanged, this, &GravitySimulation::onGSliderChanged);

    m_glWidget->setMinimumWidth(300);
    m_glWidget->setMinimumHeight(150);
    m_mainLayout->addLayout(vBoxLayout);
    m_mainLayout->addWidget(m_glWidget, 1);  // 1 - Растяжение этого виджета

    centralWidget->setLayout(m_mainLayout);
}

/**
 * @brief Запускает симуляцию гравитационного взаимодействия.
 *
 * Вызывает метод startSimulation у виджета отрисовки (SimulationGLWidget),
 * который активирует таймер обновления кадров.
 */
void GravitySimulation::onStartSimulation()
{
    m_glWidget->startSimulation();
}

/**
 * @brief Останавливает симуляцию гравитационного взаимодействия.
 *
 * Вызывает метод stopSimulation у виджета отрисовки, приостанавливая
 * расчет физики и перерисовку сцены.
 */
void GravitySimulation::onStopSimulation()
{
    m_glWidget->stopSimulation();
}

/**
 * @brief Обновляет список планет в QListWidget.
 *
 * Очищает текущий список и заново заполняет его именами всех объектов,
 * хранящихся в векторе m_objects виджета m_glWidget.
 * Вызывается при изменении состава планет (добавление, удаление, переименование).
 */
void GravitySimulation::updatePlanetList()
{
    m_listWidget->clear();

    for (const auto& obj : m_glWidget->m_objects)
    {
        m_listWidget->addItem(obj.name);
    }
}

/**
 * @brief Обработчик нажатия кнопки "Добавить планету".
 *
 * Открывает диалог AddPlanetDialog, передавая ему список существующих имен
 * для проверки уникальности. Если пользователь подтверждает ввод (Accepted),
 * создается новый объект Object и добавляется в симуляцию.
 */
void GravitySimulation::onAddPlanet()
{
    QStringList existingNames;

    for (const auto& planet : m_glWidget->m_objects)
    {
        existingNames << planet.name;
    }

    AddPlanetDialog dialog(existingNames, this);

    try
    {
        if (dialog.exec() == QDialog::Accepted)
        {
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
    catch (const InvalidPlanetDataException& e)
    {
        QMessageBox::critical(this, "Ошибка ввода данных", e.what());
        qWarning() << "Ошибка добавления планеты: " << e.what();
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, "Критическая ошибка",
                              "Произошла непредвиденная ошибка при добавлении планеты:\n" + QString::fromStdString(e.what()));
        qCritical() << "Неизвестная ошибка в onAddPlanet: " << e.what();
    }
}

/**
 * @brief Обработчик двойного клика по элементу списка планет.
 *
 * При двойном клике открывает диалог PlanetInfoDialog для просмотра/редактирования
 * информации о выбранной планете.
 *
 * Логика диалога:
 * - Если пользователь нажал OK: планета переименовывается.
 * - Если пользователь выбрал опцию удаления (возвращает -1): планета удаляется из симуляции.
 *
 * @param item Указатель на элемент списка (QListWidgetItem), по которому был сделан двойной клик.
 */
void GravitySimulation::onPlanetDoubleClicked(QListWidgetItem* item)
{
    int index = m_listWidget->row(item);
    if (index < 0 || index > m_glWidget->m_objects.size())
    {
        QMessageBox::warning(this, "Ошибка", "Выбран неверный элемент списка.");
        return;
    }

    Object& planet = m_glWidget->m_objects[index];

    PlanetInfoDialog dialog(planet, this);

    try
    {
        int result = dialog.exec();

        if (result == QDialog::Accepted)
        {
            QString newName = dialog.getNewName();
            if (!newName.isEmpty())
            {
                planet.name = newName;
                updatePlanetList();
            }
        }
        else if (result == -1)
        {
            m_glWidget->m_objects.erase(m_glWidget->m_objects.begin() + index);
            m_glWidget->update();
            updatePlanetList();
        }
    }
    catch (const InvalidPlanetDataException& e)
    {
        QMessageBox::critical(this, "Ошибка редактирования", e.what());
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, "Критическая ошибка", QString::fromStdString(e.what()));
    }
}

/**
 * @brief Обработчик изменения значения слайдера гравитационной постоянной (G).
 *
 * Преобразует целочисленное значение слайдера в реальное значение константы G
 * (умножая на 1e-12) и присваивает его полю G виджета симуляции.
 *
 * @param value Новое целочисленное значение, полученное от слайдера.
 */
void GravitySimulation::onGSliderChanged(int value)
{
    double G = value * 1e-12;
    m_glWidget->G = G;
}