#pragma once

#ifndef SIMULATIONEXCEPTIONS_H
#define SIMULATIONEXCEPTIONS_H

#include <QString>
#include <stdexcept>

class InvalidPlanetDataException : public std::runtime_error
{
   public:
    explicit InvalidPlanetDataException(const QString& msg)
        : std::runtime_error(msg.toStdString()) {}
};

class InitializationException : public std::runtime_error
{
   public:
    explicit InitializationException(const QString& msg)
        : std::runtime_error(msg.toStdString()) {}
};

class SimulationLogicException : public std::runtime_error
{
   public:
    explicit SimulationLogicException(const QString& msg)
        : std::runtime_error(msg.toStdString()) {}
};

#endif  // !SIMULATIONEXCEPTIONS_H
