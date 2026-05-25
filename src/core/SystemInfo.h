/*
 * SystemInfo.h
 * ============
 * Description: Abstract base class for all system information snapshots.
 *              Provides common attributes (source path, timestamp) and defines
 *              the interface that derived classes (CpuInfo, ProcessInfo) must implement.
 *
 * OOP Concepts Demonstrated:
 *   - Abstract base class
 *   - Encapsulation (protected members)
 *   - Virtual destructor
 *   - Pure virtual functions
 *   - Inheritance foundation
 *
 * Author: OOP 2 Project Team
 * Course: OOP 2 (MSC1052) — Spring 2026
 */

#pragma once

#include <string>
#include <ctime>
#include "IRenderable.h"

/**
 * @class SystemInfo
 * @brief Abstract base class for CPU and process information snapshots.
 *
 * Demonstrates: abstract class, protected members, virtual destructor,
 * pure virtual update() and toDisplayString(), inheritance foundation.
 *
 * Corresponds to: Lectures 6, 9
 */
// (Abstract class) SystemInfo — base of the inheritance hierarchy
class SystemInfo : public IRenderable {
protected:
    // Encapsulation: protected — accessible by derived classes only
    std::string sourcePath;    // the /proc/... path this object reads from
    time_t lastUpdated;        // Unix timestamp of the last successful read

public:
    // Parametrized constructor: initializes source path and timestamp
    explicit SystemInfo(const std::string& path);

    // Virtual destructor: ensures correct destructor chain when deleting via base pointer
    virtual ~SystemInfo();

    // Getter: returns the /proc source path
    std::string getSourcePath() const;

    // Getter: returns timestamp of last update
    time_t getLastUpdated() const;

    // Pure virtual function: derived classes must refresh their data from /proc
    virtual void update() = 0;

    // Pure virtual function: inherited from IRenderable — produce a display string
    std::string toDisplayString() const override = 0;
};
