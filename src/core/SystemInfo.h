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
 * Lecture Reference: Lecture 6 (Inheritance), Lecture 9 (Abstract Classes)
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
// [ABSTRACT CLASS] SystemInfo — base of the inheritance hierarchy
class SystemInfo : public IRenderable {
protected:
    // [ENCAPSULATION] protected — accessible by derived classes only
    std::string sourcePath;    // the /proc/... path this object reads from
    time_t lastUpdated;        // Unix timestamp of the last successful read

public:
    // [PARAMETERIZED CONSTRUCTOR] initializes source path and timestamp
    explicit SystemInfo(const std::string& path);

    // [VIRTUAL DESTRUCTOR] ensures correct destructor chain when deleting via base pointer
    virtual ~SystemInfo();

    // [GETTER] returns the /proc source path
    std::string getSourcePath() const;

    // [GETTER] returns timestamp of last update
    time_t getLastUpdated() const;

    // [PURE VIRTUAL] derived classes must refresh their data from /proc
    virtual void update() = 0;

    // [PURE VIRTUAL] inherited from IRenderable — produce a display string
    std::string toDisplayString() const override = 0;
};
