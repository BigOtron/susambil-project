/*
 * CpuReader.h
 * ===========
 * Description: Concrete implementation of IDataReader for CPU data.
 *              Opens /proc/stat, parses the cpu line, and updates a CpuInfo object.
 *
 * OOP Concepts Demonstrated:
 *   - Inheritance (implements IDataReader interface)
 *   - Composition (has-a CpuInfo, has-a ifstream)
 *   - Override of all pure virtual functions
 *   - Exception handling
 *   - File I/O (ifstream)
 *
 * Lecture Reference: Lecture 9 (Abstract), Lecture 10 (File I/O), Lecture 11 (Exceptions)
 *
 * Author: OOP 2 Project Team
 * Course: OOP 2 (MSC1052) — Spring 2026
 */

#pragma once

#include "IDataReader.h"
#include "CpuInfo.h"
#include <fstream>
#include <string>

/**
 * @class CpuReader
 * @brief Reads and parses /proc/stat to fill a CpuInfo snapshot.
 *
 * Demonstrates: interface implementation, composition, file I/O, exceptions.
 * Corresponds to: Lectures 9, 10, 11
 */
// [INHERITANCE] CpuReader implements IDataReader interface
class CpuReader : public IDataReader {
private:
    // [COMPOSITION] CpuReader HAS-A CpuInfo — stores the parsed result
    CpuInfo cpuData;

    // [COMPOSITION] CpuReader HAS-A ifstream — file handle for /proc/stat
    std::ifstream procStatFile;

    static const std::string PROC_STAT_PATH;   // "/proc/stat"

public:
    // [DEFAULT CONSTRUCTOR]
    CpuReader();

    // [DESTRUCTOR] closes any open file handle
    ~CpuReader() override;

    // [OVERRIDE] opens /proc/stat
    bool openSource() override;

    // [OVERRIDE] reads and parses the cpu line — throws on error
    void readData() override;

    // [OVERRIDE] closes the file handle
    void closeSource() override;

    // [OVERRIDE] returns true if /proc/stat is accessible
    bool isAvailable() const override;

    // [GETTER] const reference to the most recent CPU snapshot
    const CpuInfo& getCpuData() const;
};
