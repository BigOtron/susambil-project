/*
 * ProcessInfo.cpp
 * ===============
 * Description: Implementation of ProcessInfo — the most OOP-rich class in the project.
 *              Demonstrates constructors, destructors, copy constructor, operator
 *              overloads, friend function, this pointer, and inheritance.
 *
 * OOP Concepts Demonstrated:
 *   - Default, parameterized, copy constructors
 *   - Destructor
 *   - Operator overloading (<, ==, >, <<)
 *   - friend function (operator<<)
 *   - this pointer in setters
 *   - Inheritance (constructor chaining to SystemInfo)
 *   - Virtual function overrides
 *
 * Lecture Reference: Lectures 2–7
 *
 * Author: OOP 2 Project Team
 * Course: OOP 2 (MSC1052) — Spring 2026
 */

#include "ProcessInfo.h"
#include "../utils/Logger.h"
#include "../utils/Formatter.h"
#include <fstream>
#include <sstream>

// [DEFAULT CONSTRUCTOR] — creates an empty/invalid process placeholder
// [CONSTRUCTOR CHAINING] calls SystemInfo with an empty path
ProcessInfo::ProcessInfo()
    : SystemInfo(""),
      pid(0), name("unknown"), cpuUsage(0.0),
      memoryKb(0), threadCount(0), state('?')
{
}

// [PARAMETERIZED CONSTRUCTOR] — initialize with all known fields
// [CONSTRUCTOR CHAINING] builds path like /proc/PID/ for base class
ProcessInfo::ProcessInfo(int pid, const std::string& name, double cpu,
                         long mem, int threads, char state)
    : SystemInfo("/proc/" + std::to_string(pid)),
      pid(pid), name(name), cpuUsage(cpu),
      memoryKb(mem), threadCount(threads), state(state)
{
}

// [COPY CONSTRUCTOR] — deep copy of another ProcessInfo
// All members are value types (int, string, double) so memberwise copy is sufficient,
// but we demonstrate the explicit copy constructor as required by the course.
ProcessInfo::ProcessInfo(const ProcessInfo& other)
    : SystemInfo(other.sourcePath),   // [COPY CONSTRUCTOR] copy base class state
      pid(other.pid),
      name(other.name),
      cpuUsage(other.cpuUsage),
      memoryKb(other.memoryKb),
      threadCount(other.threadCount),
      state(other.state)
{
    this->lastUpdated = other.lastUpdated;   // [this POINTER] copy protected member
}

// [DESTRUCTOR] — logs when a process entry is cleaned up
ProcessInfo::~ProcessInfo() {
    // Intentionally lightweight — no dynamic memory to release
}

// ---- Getters ----------------------------------------------------------------

int ProcessInfo::getPid() const {
    return this->pid;
}

std::string ProcessInfo::getName() const {
    return this->name;
}

double ProcessInfo::getCpuUsage() const {
    return this->cpuUsage;
}

long ProcessInfo::getMemoryKb() const {
    return this->memoryKb;
}

int ProcessInfo::getThreadCount() const {
    return this->threadCount;
}

char ProcessInfo::getState() const {
    return this->state;
}

// ---- Setters ----------------------------------------------------------------

// [this POINTER] used explicitly to distinguish member from parameter
void ProcessInfo::setCpuUsage(double cpu) {
    this->cpuUsage = cpu;
}

void ProcessInfo::setMemoryKb(long mem) {
    this->memoryKb = mem;
}

void ProcessInfo::setThreadCount(int threads) {
    this->threadCount = threads;
}

void ProcessInfo::setState(char s) {
    this->state = s;
}

void ProcessInfo::setName(const std::string& n) {
    this->name = n;
}

// [OVERRIDE] reads /proc/PID/status to refresh memory, threads, state, name
void ProcessInfo::update() {
    std::string statusPath = "/proc/" + std::to_string(this->pid) + "/status";
    std::ifstream statusFile(statusPath);
    if (!statusFile.is_open()) {
        return;   // process may have exited — silently skip
    }

    std::string key;
    std::string value;
    while (statusFile >> key) {
        if (key == "Name:") {
            statusFile >> this->name;
        } else if (key == "State:") {
            std::string stateStr;
            statusFile >> stateStr;
            this->state = stateStr.empty() ? '?' : stateStr[0];
        } else if (key == "VmRSS:") {
            statusFile >> this->memoryKb;
        } else if (key == "Threads:") {
            statusFile >> this->threadCount;
        } else {
            std::string rest;
            std::getline(statusFile, rest);   // skip rest of line
        }
    }
    statusFile.close();
    this->lastUpdated = time(nullptr);
}

// [OVERRIDE] produces a debug-friendly one-line description
std::string ProcessInfo::toDisplayString() const {
    return "PID=" + std::to_string(this->pid)
         + " NAME=" + this->name
         + " CPU=" + formatDouble(this->cpuUsage) + "%"
         + " MEM=" + std::to_string(this->memoryKb) + "KB";
}

// ---- Operator overloads -----------------------------------------------------

// [OPERATOR OVERLOAD] less-than: compare by CPU usage — used by std::sort
bool ProcessInfo::operator<(const ProcessInfo& other) const {
    return this->cpuUsage < other.cpuUsage;
}

// [OPERATOR OVERLOAD] equality: compare by PID — used by findIndex<T>
bool ProcessInfo::operator==(const ProcessInfo& other) const {
    return this->pid == other.pid;
}

// [OPERATOR OVERLOAD] greater-than: convenience for descending CPU sort
bool ProcessInfo::operator>(const ProcessInfo& other) const {
    return this->cpuUsage > other.cpuUsage;
}

// [FRIEND FUNCTION] stream insertion operator — accesses private members directly
// This is a non-member function declared friend in ProcessInfo
std::ostream& operator<<(std::ostream& os, const ProcessInfo& p) {
    os << "[PID=" << p.pid
       << " NAME=" << p.name
       << " CPU=" << p.cpuUsage << "%"
       << " MEM=" << p.memoryKb << "KB"
       << " THREADS=" << p.threadCount
       << " STATE=" << p.state << "]";
    return os;
}
