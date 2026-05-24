/*
 * ProcessReader.cpp
 * =================
 * Description: Scans /proc for all running PIDs and reads their status files.
 *              Computes per-process CPU usage using /proc/PID/stat tick deltas.
 *
 * OOP Concepts Demonstrated:
 *   - Inheritance (implements IDataReader)
 *   - File I/O (ifstream, dirent)
 *   - Exception handling (try-catch-throw, multiple catch blocks)
 *   - Static member and static helper function
 *   - Composition (populates a ProcessList — has-a relationship at call site)
 *
 * Lecture Reference: Lecture 10 (File I/O), Lecture 11 (Exceptions)
 *
 * Author: OOP 2 Project Team
 * Course: OOP 2 (MSC1052) — Spring 2026
 */

#include "ProcessReader.h"
#include "MonitorExceptions.h"
#include "../utils/Logger.h"
#include "../utils/Formatter.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <dirent.h>
#include <cctype>

// [STATIC MEMBER] path to the /proc virtual filesystem
const std::string ProcessReader::PROC_DIR = "/proc";

// [DEFAULT CONSTRUCTOR]
ProcessReader::ProcessReader()
    : prevSystemTotal(0), currSystemTotal(0)
{
    Logger::getInstance()->logInfo("ProcessReader constructed");
}

// [DESTRUCTOR]
ProcessReader::~ProcessReader() {
    Logger::getInstance()->logInfo("ProcessReader destroyed");
}

// [OVERRIDE] check /proc is accessible
bool ProcessReader::openSource() {
    DIR* dir = opendir(PROC_DIR.c_str());
    if (!dir) return false;
    closedir(dir);
    return true;
}

// [OVERRIDE] simple single-read; use readAll() for the full scan
void ProcessReader::readData() {
    // Intentionally thin — readAll() is the main entry point
}

// [OVERRIDE] nothing to close (we open/close inside readAll per-iteration)
void ProcessReader::closeSource() {}

// [OVERRIDE] check /proc directory is accessible
bool ProcessReader::isAvailable() const {
    DIR* dir = opendir(PROC_DIR.c_str());
    if (!dir) return false;
    closedir(dir);
    return true;
}

// Primary method — scans /proc, fills processList, then sorts by CPU descending
// [EXCEPTION HANDLING] wraps the entire scan in try-catch
void ProcessReader::readAll(ProcessList& processList) {
    try {
        processList.clear();

        // Read total CPU ticks from /proc/stat to compute per-process CPU%
        std::ifstream statFile("/proc/stat");
        if (!statFile.is_open()) {
            throw FileReadException("/proc/stat");
        }
        std::string cpuLine;
        std::getline(statFile, cpuLine);
        statFile.close();

        unsigned long long u, n, s, id, io, ir, si, st, g, gn;
        std::istringstream css(cpuLine);
        std::string tag;
        css >> tag >> u >> n >> s >> id >> io >> ir >> si >> st >> g >> gn;
        prevSystemTotal = currSystemTotal;
        currSystemTotal = u + n + s + id + io + ir + si;
        unsigned long long systemDelta = currSystemTotal - prevSystemTotal;
        if (systemDelta == 0) systemDelta = 1;   // avoid division by zero

        // Open /proc directory and iterate over numeric entries
        DIR* procDir = opendir(PROC_DIR.c_str());
        if (!procDir) {
            throw FileReadException(PROC_DIR);
        }

        struct dirent* entry;
        while ((entry = readdir(procDir)) != nullptr) {
            std::string dirName(entry->d_name);

            // Only process numeric directory names (PIDs)
            if (!isNumeric(dirName)) continue;

            int pid = std::stoi(dirName);

            try {
                // [EXCEPTION] inner try so one bad process doesn't abort the scan
                ProcessInfo info = readProcessStatus(pid);

                // Compute this process's CPU share using /proc/PID/stat
                unsigned long long procTicks = readProcessCpuTicks(pid);
                // [TEMPLATE] calculatePercent<unsigned long long>
                double cpuPct = calculatePercent<unsigned long long>(procTicks, systemDelta);
                cpuPct = clampValue<double>(cpuPct, 0.0, 100.0);
                info.setCpuUsage(cpuPct);

                processList.add(info);

            } catch (const ProcessNotFoundException&) {
                // Process exited between opendir and readdir — silently skip
            } catch (const ParseException& e) {
                Logger::getInstance()->logWarning(
                    "Skip PID " + dirName + ": " + e.what());
            } catch (const std::exception& e) {
                Logger::getInstance()->logWarning(
                    "Skip PID " + dirName + ": " + e.what());
            }
        }
        closedir(procDir);

        // Sort by CPU descending so the busiest processes appear at the top
        processList.sortByCpu();

    } catch (const FileReadException& e) {
        Logger::getInstance()->logError(e.what());
    } catch (const std::exception& e) {
        Logger::getInstance()->logError(std::string("ProcessReader::readAll: ") + e.what());
    }
}

// Reads /proc/PID/status and constructs a ProcessInfo from it
ProcessInfo ProcessReader::readProcessStatus(int pid) {
    std::string path = PROC_DIR + "/" + std::to_string(pid) + "/status";

    // [FILE I/O] open the status file
    std::ifstream file(path);
    if (!file.is_open()) {
        throw ProcessNotFoundException(pid);
    }

    std::string name;
    char state = '?';
    long memKb = 0;
    int threads = 0;

    std::string line;
    while (std::getline(file, line)) {
        if (line.rfind("Name:", 0) == 0) {
            std::istringstream ss(line.substr(5));
            ss >> name;
        } else if (line.rfind("State:", 0) == 0) {
            // State line: "State:	S (sleeping)"
            std::size_t pos = line.find_first_not_of(" \t", 6);
            if (pos != std::string::npos) state = line[pos];
        } else if (line.rfind("VmRSS:", 0) == 0) {
            std::istringstream ss(line.substr(6));
            ss >> memKb;
        } else if (line.rfind("Threads:", 0) == 0) {
            std::istringstream ss(line.substr(8));
            ss >> threads;
        }
    }
    file.close();   // [FILE I/O] explicit close

    if (name.empty()) {
        throw ParseException("Empty Name in PID " + std::to_string(pid) + " status");
    }

    return ProcessInfo(pid, name, 0.0, memKb, threads, state);
}

// Reads utime + stime from /proc/PID/stat — returns 0 on any failure
unsigned long long ProcessReader::readProcessCpuTicks(int pid) {
    std::string path = PROC_DIR + "/" + std::to_string(pid) + "/stat";
    std::ifstream file(path);
    if (!file.is_open()) return 0;

    // /proc/PID/stat fields (space-separated):
    // 1:pid 2:comm 3:state 4:ppid ... 14:utime 15:stime ...
    std::string content;
    std::getline(file, content);
    file.close();

    // Find the closing ')' of the comm field — name may contain spaces
    std::size_t commEnd = content.rfind(')');
    if (commEnd == std::string::npos) return 0;

    std::istringstream ss(content.substr(commEnd + 1));
    std::string state;
    long ppid, pgrp, session, tty, tpgid;
    unsigned long flags, minflt, cminflt, majflt, cmajflt, utime, stime;

    ss >> state >> ppid >> pgrp >> session >> tty >> tpgid
       >> flags >> minflt >> cminflt >> majflt >> cmajflt
       >> utime >> stime;

    return utime + stime;
}

// Returns true if every character in str is a decimal digit
bool ProcessReader::isNumeric(const std::string& str) {
    if (str.empty()) return false;
    return std::all_of(str.begin(), str.end(), ::isdigit);
}
