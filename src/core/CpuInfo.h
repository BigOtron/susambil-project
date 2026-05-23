/*
 * CpuInfo.h
 * =========
 * Description: Derived class that holds a single CPU usage snapshot.
 *              Reads raw tick values from /proc/stat and computes usage percentage
 *              by comparing against the previous snapshot.
 *
 * OOP Concepts Demonstrated:
 *   - Inheritance (derived from SystemInfo)
 *   - Encapsulation (private tick fields)
 *   - Override of pure virtual functions
 *   - Constructor chaining (calls SystemInfo constructor)
 *
 * Lecture Reference: Lecture 6 (Inheritance), Lecture 7 (Polymorphism)
 *
 * Author: OOP 2 Project Team
 * Course: OOP 2 (MSC1052) — Spring 2026
 */

#pragma once

#include "SystemInfo.h"

/**
 * @class CpuInfo
 * @brief Stores CPU tick counters and computes usage percentage.
 *
 * Demonstrates: inheritance from SystemInfo, encapsulation, virtual overrides,
 * constructor chaining.
 *
 * Corresponds to: Lectures 6, 7
 */
// [INHERITANCE] CpuInfo IS-A SystemInfo
class CpuInfo : public SystemInfo {
private:
    // [ENCAPSULATION] raw tick values from /proc/stat — current snapshot
    unsigned long long user;
    unsigned long long nice;
    unsigned long long system;
    unsigned long long idle;
    unsigned long long iowait;
    unsigned long long irq;
    unsigned long long softirq;

    // previous snapshot — used to compute delta
    unsigned long long prevUser;
    unsigned long long prevNice;
    unsigned long long prevSystem;
    unsigned long long prevIdle;
    unsigned long long prevIowait;
    unsigned long long prevIrq;
    unsigned long long prevSoftirq;

    double usagePercent;   // computed CPU usage in [0, 100]

public:
    // [DEFAULT CONSTRUCTOR] — calls SystemInfo("/proc/stat")
    CpuInfo();

    // [DESTRUCTOR] logs cleanup
    ~CpuInfo() override;

    // [OVERRIDE] reads /proc/stat and updates tick values
    void update() override;

    // [OVERRIDE] returns "CPU: XX.X%"
    std::string toDisplayString() const override;

    // [GETTER] CPU usage percentage
    double getUsagePercent() const;

    // [GETTER] total active ticks in current snapshot
    unsigned long long getTotalTicks() const;

    // [GETTER] idle ticks in current snapshot
    unsigned long long getIdleTicks() const;

    /**
     * @brief Updates raw tick values from a parsed /proc/stat line.
     * Called by CpuReader after it parses the file.
     */
    void setTicks(unsigned long long u, unsigned long long n,
                  unsigned long long s, unsigned long long id,
                  unsigned long long io, unsigned long long ir,
                  unsigned long long si);

    /**
     * @brief Recalculates usagePercent from current vs previous snapshots.
     * Must be called after setTicks() with the new values already stored.
     */
    void calculateUsage();
};
