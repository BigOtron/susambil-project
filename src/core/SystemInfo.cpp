/*
 * SystemInfo.cpp
 * ==============
 * Description: Implementation of the SystemInfo abstract base class.
 *              Provides constructor, destructor, and accessor implementations.
 *
 * OOP Concepts Demonstrated:
 *   - Abstract base class implementation
 *   - Constructor initialization list
 *   - Virtual destructor definition
 *   - Encapsulation via getters
 *
 * Author: OOP 2 Project Team
 * Course: OOP 2 (MSC1052) — Spring 2026
 */

#include "SystemInfo.h"
#include "../utils/Logger.h"

// Parametrized constructor: initializes members via initialization list
SystemInfo::SystemInfo(const std::string& path)
    : sourcePath(path), lastUpdated(0)
{
    // Log construction for debugging
    Logger::getInstance()->logInfo("SystemInfo created for path: " + path);
}

// Virtual destructor: ensures correct cleanup order in polymorphic hierarchies
SystemInfo::~SystemInfo() {
    Logger::getInstance()->logInfo("SystemInfo destroyed for path: " + sourcePath);
}

// Getter: returns the /proc source path string
std::string SystemInfo::getSourcePath() const {
    return this->sourcePath;   // [this POINTER] explicit this-> for clarity
}

// Getter: returns Unix timestamp of the last successful data read
time_t SystemInfo::getLastUpdated() const {
    return this->lastUpdated;   // [this POINTER] explicit this-> for clarity
}
