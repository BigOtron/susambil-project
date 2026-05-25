/*
* ProcessInfo.h
 * =============
 * This header defines the ProcessInfo class — one process entry from /proc/[pid]/.
 * It's the most feature-rich class in the project, so if you're looking for
 * examples of OOP concepts in action, start here.
 *
 * You'll find: encapsulation, all three constructor types, a destructor,
 * operator overloads, a friend function, this pointer, and inheritance.
 *
 * Covers material from Lectures 2–7.
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
 * @brief Holds all the data we care about for a single running Linux process.
 *
 * Inherits from SystemInfo which handles the /proc path and last-updated timestamp.
 * This class adds the process-specific stuff: PID, name, CPU, memory, threads, state.
 */

// [INHERITANCE] ProcessInfo extends SystemInfo — every process IS a system info entry
class ProcessInfo : public SystemInfo {
private:
    // [ENCAPSULATION] Keep these private so nothing outside the class can mess with them directly
    int         pid;          // Process ID — unique identifier assigned by the OS
    std::string name;         // Process name, e.g. "firefox" or "bash"
    double      cpuUsage;     // How much CPU this process is currently eating (%)
    long        memoryKb;     // Physical RAM in use, pulled from VmRSS in /proc
    int         threadCount;  // How many threads this process is running
    char        state;        // One-letter state: R=running, S=sleeping, Z=zombie, etc.

public:
    // [DEFAULT CONSTRUCTOR] Creates a blank, invalid placeholder — useful when you need the object
    // before you actually have data to fill it with
    ProcessInfo();

    // [PARAMETERIZED CONSTRUCTOR] The one you'll use most of the time — pass in everything you know upfront
    ProcessInfo(int pid, const std::string& name, double cpu,
                long mem, int threads, char state);

    // [COPY CONSTRUCTOR] Makes an independent copy of another ProcessInfo object
    ProcessInfo(const ProcessInfo& other);

    // [DESTRUCTOR] nothing dramatic, just here to complete the class properly
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

    // [OVERRIDE] Re-reads /proc/PID/status and updates all fields with fresh values
    void update() override;

    // [OVERRIDE] Returns a compact summary string like "PID=123 NAME=bash CPU=0.5%"
    std::string toDisplayString() const override;

    // ---- Operator overloads ----

    // [OPERATOR OVERLOAD] Sort by CPU usage — lower CPU comes first
    bool operator<(const ProcessInfo& other) const;

    // [OPERATOR OVERLOAD] Two processes are "the same" if they have the same PID
    bool operator==(const ProcessInfo& other) const;

    // [OPERATOR OVERLOAD] Higher CPU comes first — useful for descending sort
    bool operator>(const ProcessInfo& other) const;

    // [FRIEND FUNCTION] Lets you write: cout << myProcess; and get a nicely formatted output
    // Needs to be a friend so it can read the private fields
    friend std::ostream& operator<<(std::ostream& os, const ProcessInfo& p);

    // [FRIEND CLASS] // ProcessList gets direct access to our internals to avoid unnecessary getter overhead
    friend class ProcessList;
};
