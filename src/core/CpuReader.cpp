/*
 * CpuReader.cpp
 * =============
 * Description: Implementation of CpuReader — opens /proc/stat, parses the cpu line,
 *              and updates a CpuInfo snapshot. Demonstrates file I/O and exception handling.
 *
 * OOP Concepts Demonstrated:
 *   - Inheritance (implements IDataReader)
 *   - Composition (has-a CpuInfo, has-a ifstream)
 *   - File I/O (ifstream open/read/close)
 *   - Exception handling (try-catch-throw with custom exceptions)
 *   - Multiple catch blocks (specific to general order)
 *   - Static member access
 *
 * Lecture Reference: Lecture 10 (File I/O), Lecture 11 (Exceptions)
 *
 * Author: OOP 2 Project Team
 * Course: OOP 2 (MSC1052) — Spring 2026
 */

#include "CpuReader.h"
#include "MonitorExceptions.h"
#include "../utils/Logger.h"
#include <sstream>

// [STATIC MEMBER] definition of the path constant
const std::string CpuReader::PROC_STAT_PATH = "/proc/stat";

// [DEFAULT CONSTRUCTOR] — CpuInfo is default-constructed as a composed member
CpuReader::CpuReader() {
    Logger::getInstance()->logInfo("CpuReader constructed");
}

// [DESTRUCTOR] closes any lingering file handle and logs cleanup
CpuReader::~CpuReader() {
    if (procStatFile.is_open()) {
        procStatFile.close();   // [FILE I/O] ensure file is closed on destruction
    }
    Logger::getInstance()->logInfo("CpuReader destroyed");
}

// [OVERRIDE] opens /proc/stat for reading
bool CpuReader::openSource() {
    // [FILE I/O] open the proc file
    procStatFile.open(PROC_STAT_PATH);
    if (!procStatFile.is_open()) {
        Logger::getInstance()->logError("CpuReader: cannot open " + PROC_STAT_PATH);
        return false;
    }
    return true;
}

// [OVERRIDE] reads and parses the first cpu line from /proc/stat
// [EXCEPTION HANDLING] try-catch-throw with multiple catch blocks
void CpuReader::readData() {
    try {
        // [FILE I/O] open the file fresh on each read for simplicity
        std::ifstream file(PROC_STAT_PATH);
        if (!file.is_open()) {
            // [EXCEPTION] throw custom exception when file is inaccessible
            throw FileReadException(PROC_STAT_PATH);
        }

        std::string line;
        if (!std::getline(file, line)) {
            // [EXCEPTION] throw parse exception on empty file
            throw ParseException("Empty /proc/stat");
        }

        // Validate the line starts with "cpu"
        if (line.size() < 3 || line.substr(0, 3) != "cpu") {
            throw ParseException("Unexpected /proc/stat format: " + line);
        }

        // Parse raw tick values from the cpu line
        // Format: cpu  user nice system idle iowait irq softirq steal guest guestNice
        std::istringstream ss(line);
        std::string tag;
        unsigned long long u, n, s, id, io, ir, si, steal, guest, guestNice;
        if (!(ss >> tag >> u >> n >> s >> id >> io >> ir >> si >> steal >> guest >> guestNice)) {
            throw ParseException("Cannot parse cpu fields in /proc/stat");
        }

        file.close();   // [FILE I/O] explicit close

        // [COMPOSITION] update the composed CpuInfo object with new tick values
        cpuData.setTicks(u, n, s, id, io, ir, si);
        cpuData.calculateUsage();

    } catch (const FileReadException& e) {
        // [EXCEPTION] catch most specific type first
        Logger::getInstance()->logError(std::string(e.what()));
    } catch (const ParseException& e) {
        // [EXCEPTION] catch parse errors
        Logger::getInstance()->logError(std::string(e.what()));
    } catch (const std::exception& e) {
        // [EXCEPTION] catch all remaining standard exceptions — most general last
        Logger::getInstance()->logError(std::string("CpuReader unexpected: ") + e.what());
    }
}

// [OVERRIDE] closes the persistent file handle if open
void CpuReader::closeSource() {
    if (procStatFile.is_open()) {
        procStatFile.close();   // [FILE I/O] explicit close
    }
}

// [OVERRIDE] checks whether /proc/stat exists and can be opened
bool CpuReader::isAvailable() const {
    std::ifstream test(PROC_STAT_PATH);
    return test.is_open();
}

// [GETTER] returns const reference to the composed CpuInfo — avoids copying
const CpuInfo& CpuReader::getCpuData() const {
    return this->cpuData;
}
