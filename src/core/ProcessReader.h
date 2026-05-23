/*
 * ProcessReader.h
 * ===============
 * Description: Concrete implementation of IDataReader for process data.
 *              Scans /proc for numeric directories (PIDs) and reads status files.
 *
 * OOP Concepts Demonstrated:
 *   - Inheritance (implements IDataReader)
 *   - Composition (has-a ProcessList)
 *   - File I/O (ifstream, dirent directory scanning)
 *   - Exception handling
 *   - Static helper method
 *
 * Lecture Reference: Lecture 9 (Abstract), Lecture 10 (File I/O), Lecture 11 (Exceptions)
 *
 * Author: OOP 2 Project Team
 * Course: OOP 2 (MSC1052) — Spring 2026
 */

#pragma once

#include "IDataReader.h"
#include "ProcessList.h"
#include <string>

/**
 * @class ProcessReader
 * @brief Scans /proc directory and reads process info for each PID.
 *
 * Demonstrates: interface implementation, composition, file I/O, exceptions.
 * Corresponds to: Lectures 9, 10, 11
 */
// [INHERITANCE] ProcessReader implements IDataReader
class ProcessReader : public IDataReader {
private:
    static const std::string PROC_DIR;   // "/proc"

    // Previous CPU tick totals per PID — used to compute per-process CPU delta
    // Maps PID → (prevUtime + prevStime)
    // We store these as parallel data inside ProcessInfo or as a simple map
    // For simplicity we recompute from /proc/PID/stat each tick
    unsigned long long prevSystemTotal;   // previous total system ticks (for CPU%)
    unsigned long long currSystemTotal;   // current total system ticks

public:
    // [DEFAULT CONSTRUCTOR]
    ProcessReader();

    // [DESTRUCTOR]
    ~ProcessReader() override;

    // [OVERRIDE] checks /proc is accessible
    bool openSource() override;

    // [OVERRIDE] scans /proc and populates the provided ProcessList
    void readData() override;

    // [OVERRIDE] no persistent handle to close
    void closeSource() override;

    // [OVERRIDE] returns true if /proc is a readable directory
    bool isAvailable() const override;

    /**
     * @brief Scans /proc, fills processList with fresh data, and sorts by CPU.
     * This is the primary entry point called by MainWindow each tick.
     */
    void readAll(ProcessList& processList);

private:
    // Reads /proc/PID/status and returns a ProcessInfo (throws on parse failure)
    ProcessInfo readProcessStatus(int pid);

    // Reads utime+stime from /proc/PID/stat (returns 0 on failure)
    unsigned long long readProcessCpuTicks(int pid);

    // Returns true if the string contains only digit characters
    static bool isNumeric(const std::string& str);
};
