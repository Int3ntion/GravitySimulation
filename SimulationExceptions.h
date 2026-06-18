/**
 * @file SimulationExceptions.h
 * @brief Определение пользовательских исключений для гравитационной симуляции.
 *
 * Данный заголовок содержит классы исключений.
 * Все исключения наследуются от std::runtime_error для
 * совместимости со стандартной обработкой ошибок C++, но принимают QString
 * для удобной локализации сообщений об ошибках и интеграции с Qt.
 *
 * Классы исключений позволяют четко разделять типы ошибок:
 * - InvalidPlanetDataException: ошибки валидации входных данных (масса, координаты).
 * - InitializationException: критические ошибки инициализации ресурсов (OpenGL, таймеры).
 * - SimulationLogicException: ошибки внутренней логики симуляции (деление на ноль,
 *   некорректные параметры сетки гравитационного поля).
 *
 * @author Шестаков Денис Сергеевич
 */

#pragma once

#ifndef SIMULATIONEXCEPTIONS_H
#define SIMULATIONEXCEPTIONS_H

#include <QString>
#include <stdexcept>

/**
 * @class InvalidPlanetDataException
 * @brief Исключение, выбрасываемое при попытке создания планеты с некорректными данными.
 *
 * Используется в модулях ввода данных (например, AddPlanetDialog) и валидации
 * перед добавлением объекта в симуляцию. Сценарии выброса:
 * - Отрицательная масса или радиус.
 * - Некорректные координаты (NaN, Inf).
 *
 * Наследуется от std::runtime_error. Конструктор автоматически конвертирует
 * QString в std::string для совместимости со стандартным интерфейсом исключений.
 */
class InvalidPlanetDataException : public std::runtime_error
{
   public:
    /**
     * @brief Конструктор исключения.
     *
     * Инициализирует базовое исключение std::runtime_error сообщением,
     * преобразованным из QString в std::string.
     *
     * @param msg Текст ошибки, переданный через QString.
     */
    explicit InvalidPlanetDataException(const QString& msg)
        : std::runtime_error(msg.toStdString()) {}
};

/**
 * @class InitializationException
 * @brief Исключение, выбрасываемое при неудачной инициализации компонентов симуляции.
 *
 * Возникает на этапе запуска приложения или создания виджета OpenGL.
 * Сценарии использования:
 * - Ошибка создания контекста OpenGL.
 * - Не удалось выделить память для буферов вершин.
 * - Сбой инициализации таймера обновления кадра.
 */
class InitializationException : public std::runtime_error
{
   public:
    /**
     * @brief Конструктор исключения.
     *
     * @param msg Подробное описание причины сбоя инициализации.
     */
    explicit InitializationException(const QString& msg)
        : std::runtime_error(msg.toStdString()) {}
};

/**
 * @class SimulationLogicException
 * @brief Исключение, сигнализирующее о нарушении логики работы симуляции во время выполнения.
 *
 * Выбрасывается внутри вычислительных методов (calculateForces, calculateGravityField)
 * при обнаружении аномальных состояний, которые делают дальнейшие расчеты невозможными
 * или бессмысленными.
 *
 * Сценарии использования:
 * - Попытка расчета гравитационного поля с недопустимым разрешением сетки (0 или отрицательное).
 * - Обнаружение NaN или Inf в векторах скорости/позиции.
 * - Деление на ноль в физических формулах.
 */
class SimulationLogicException : public std::runtime_error
{
   public:
    /**
     * @brief Конструктор исключения.
     *
     * @param msg Описание логической ошибки или условия, которое привело к сбою.
     */
    explicit SimulationLogicException(const QString& msg)
        : std::runtime_error(msg.toStdString()) {}
};

#endif  // !SIMULATIONEXCEPTIONS_H
