/*
* ProcessInfo.cpp
 * ===============
 * This file handles everything related to a single running process —
 * how it's created, copied, updated, compared, and printed.
 *
 * It's the most "OOP-heavy" class in the whole project. If you want
 * to see constructors, destructors, operator overloads, and inheritance
 * all in one place, this is the file to look at.
 *
 * Things you'll find here:
 *   - Default, parameterized, and copy constructors
 *   - Destructor
 *   - Operator overloads (<, ==, >, <<)
 *   - A friend function for stream output
 *   - Use of "this" pointer in setters
 *   - Constructor chaining up to SystemInfo
 *   - Virtual function overrides
 *
 * Author: OOP 2 Project Team
 * Course: OOP 2 (MSC1052) — Spring 2026
 */


#include "ProcessInfo.h"
#include "../utils/Logger.h"
#include "../utils/Formatter.h"
#include <fstream>
#include <sstream>

// Default constructor — makes a blank process object that doesn't point to anything real.
// We still call the SystemInfo constructor, just with an empty path.
ProcessInfo::ProcessInfo()
    : SystemInfo(""),
      pid(0), name("unknown"), cpuUsage(0.0),
      memoryKb(0), threadCount(0), state('?')
{
}

// Main constructor — use this when you actually know the process details.
// Builds the /proc/PID/ path automatically and passes it up to SystemInfo.
ProcessInfo::ProcessInfo(int pid, const std::string& name, double cpu,
                         long mem, int threads, char state)
    : SystemInfo("/proc/" + std::to_string(pid)),
      pid(pid), name(name), cpuUsage(cpu),
      memoryKb(mem), threadCount(threads), state(state)
{
}

// Copy constructor — makes an identical copy of an existing ProcessInfo.
// Since all members are plain value types (int, string, etc.), a memberwise copy
// would technically work fine, but we wrote this out explicitly for the course.
ProcessInfo::ProcessInfo(const ProcessInfo& other)
    : SystemInfo(other.sourcePath),   // also copy the base class's path
      pid(other.pid),
      name(other.name),
      cpuUsage(other.cpuUsage),
      memoryKb(other.memoryKb),
      threadCount(other.threadCount),
      state(other.state)
{
    this->lastUpdated = other.lastUpdated;   // copy the timestamp from the base class
}

// Destructor — nothing special to clean up here since we're not using
// any raw pointers or dynamic memory.
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

// Using "this->" here to make it obvious we're assigning to the member,
// not some local variable with the same name.
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

// Reads /proc/PID/status to refresh this process's info (name, memory, threads, state).
// If the file can't be opened, we assume the process already exited and just move on.
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

// Returns a short one-liner with the key stats — mostly useful for logging and debugging.
std::string ProcessInfo::toDisplayString() const {
    return "PID=" + std::to_string(this->pid)
         + " NAME=" + this->name
         + " CPU=" + formatDouble(this->cpuUsage) + "%"
         + " MEM=" + std::to_string(this->memoryKb) + "KB";
}

// ---- Operator overloads -----------------------------------------------------

// Less-than compares by CPU usage — this is what std::sort uses by default.
bool ProcessInfo::operator<(const ProcessInfo& other) const {
    return this->cpuUsage < other.cpuUsage;
}

// Equality checks by PID — two entries are the "same" process if they share a PID.
// Used in findIndex<T> to locate a process in a list.
bool ProcessInfo::operator==(const ProcessInfo& other) const {
    return this->pid == other.pid;
}

// Greater-than is just the flip of less-than — handy for sorting highest CPU first.
bool ProcessInfo::operator>(const ProcessInfo& other) const {
    return this->cpuUsage > other.cpuUsage;
}

// Stream output operator — lets you do things like: cout << myProcess;
// Declared as a friend so it can reach the private members directly.
// This is a free function, not a method, which is the standard way to do this in C++.
std::ostream& operator<<(std::ostream& os, const ProcessInfo& p) {
    os << "[PID=" << p.pid
       << " NAME=" << p.name
       << " CPU=" << p.cpuUsage << "%"
       << " MEM=" << p.memoryKb << "KB"
       << " THREADS=" << p.threadCount
       << " STATE=" << p.state << "]";
    return os;
}
