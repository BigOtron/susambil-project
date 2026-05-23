/*
 * ProcessInfo.h
 * =============
 * Description: Represents a single Linux process entry read from /proc/[pid]/.
 *              Demonstrates the most comprehensive set of OOP concepts in the project.
 *
 * OOP Concepts Demonstrated:
 *   - Encapsulation (private members + getters/setters)
 *   - Default, parameterized, and copy constructors
 *   - Destructor
 *   - Operator overloading (<, ==, <<)
 *   - friend function
 *   - this pointer
 *   - Inheritance from SystemInfo
 *
 * Lecture Reference: Lectures 2–7
 *
 * Author: OOP 2 Project Team
 * Course: OOP 2 (MSC1052) — Spring 2026
 */

#pragma once

#include "SystemInfo.h"
#include <string>
#include <iostream>

/**
 * @class ProcessInfo
 * @brief Stores all information about a single Linux process.
 *
 * Demonstrates: encapsulation, constructors/destructors, copy constructor,
 * operator overloading, friend functions, inheritance.
 *
 * Corresponds to: Lectures 2, 3, 4, 5, 6
 */
// [INHERITANCE] ProcessInfo IS-A SystemInfo
class ProcessInfo : public SystemInfo {
private:
    // [ENCAPSULATION] all data members are private
    int         pid;          // Process ID
    std::string name;         // Process name (from /proc/PID/status Name:)
    double      cpuUsage;     // CPU usage percentage
    long        memoryKb;     // Resident memory in kilobytes (VmRSS)
    int         threadCount;  // Number of threads (from Threads: field)
    char        state;        // Process state: R, S, D, Z, T

public:
    // [DEFAULT CONSTRUCTOR] creates an empty/invalid process entry
    ProcessInfo();

    // [PARAMETERIZED CONSTRUCTOR] initialize with all fields
    ProcessInfo(int pid, const std::string& name, double cpu,
                long mem, int threads, char state);

    // [COPY CONSTRUCTOR] deep copy of another ProcessInfo
    ProcessInfo(const ProcessInfo& other);

    // [DESTRUCTOR] logs cleanup
    ~ProcessInfo() override;

    // ---- Getters ----
    int         getPid()         const;
    std::string getName()        const;
    double      getCpuUsage()    const;
    long        getMemoryKb()    const;
    int         getThreadCount() const;
    char        getState()       const;

    // ---- Setters ----
    void setCpuUsage(double cpu);
    void setMemoryKb(long mem);
    void setThreadCount(int threads);
    void setState(char s);
    void setName(const std::string& n);

    // [OVERRIDE] refreshes this process's data from /proc/PID/
    void update() override;

    // [OVERRIDE] returns "PID=X NAME=Y CPU=Z%"
    std::string toDisplayString() const override;

    // ---- Operator overloads ----

    // [OPERATOR OVERLOAD] compare by CPU usage for sorting
    bool operator<(const ProcessInfo& other) const;

    // [OPERATOR OVERLOAD] compare by PID for equality checks
    bool operator==(const ProcessInfo& other) const;

    // [OPERATOR OVERLOAD] greater-than by CPU usage
    bool operator>(const ProcessInfo& other) const;

    // [FRIEND FUNCTION] stream output — accesses private members directly
    friend std::ostream& operator<<(std::ostream& os, const ProcessInfo& p);

    // [FRIEND CLASS] ProcessList may access private members for efficiency
    friend class ProcessList;
};
