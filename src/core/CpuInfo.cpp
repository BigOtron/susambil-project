/*
 * CpuInfo.cpp
 * ===========
 * Description: Implementation of CpuInfo — stores CPU tick counters from /proc/stat
 *              and calculates CPU usage percentage via delta comparison.
 *
 * OOP Concepts Demonstrated:
 *   - Inheritance (derived from SystemInfo)
 *   - Constructor chaining (CpuInfo() : SystemInfo("/proc/stat"))
 *   - Virtual function overrides
 *   - Encapsulation
 *   - this pointer
 *
 * Lecture Reference: Lecture 6 (Inheritance), Lecture 7 (Polymorphism)
 *
 * Author: OOP 2 Project Team
 * Course: OOP 2 (MSC1052) — Spring 2026
 */

#include "CpuInfo.h"
#include "../utils/Logger.h"
#include "../utils/Formatter.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

// [CONSTRUCTOR CHAINING] CpuInfo delegates base initialization to SystemInfo
// [DEFAULT CONSTRUCTOR] all tick fields initialized to zero
CpuInfo::CpuInfo()
    : SystemInfo("/proc/stat"),
      user(0), nice(0), system(0), idle(0), iowait(0), irq(0), softirq(0),
      prevUser(0), prevNice(0), prevSystem(0), prevIdle(0),
      prevIowait(0), prevIrq(0), prevSoftirq(0),
      usagePercent(0.0)
{
    Logger::getInstance()->logInfo("CpuInfo constructed");
}

// [DESTRUCTOR] logs cleanup — override ensures correct virtual dispatch
CpuInfo::~CpuInfo() {
    Logger::getInstance()->logInfo("CpuInfo destroyed");
}

// [OVERRIDE] reads /proc/stat directly and recalculates usage
// This override satisfies the pure virtual declared in SystemInfo
void CpuInfo::update() {
    std::ifstream file(this->sourcePath);   // [this POINTER] access protected member
    if (!file.is_open()) {
        Logger::getInstance()->logError("CpuInfo::update — cannot open " + this->sourcePath);
        return;
    }

    std::string line;
    std::getline(file, line);
    file.close();

    if (line.substr(0, 3) != "cpu") {
        Logger::getInstance()->logError("CpuInfo::update — unexpected /proc/stat format");
        return;
    }

    std::istringstream ss(line);
    std::string tag;
    unsigned long long u, n, s, id, io, ir, si, steal, guest, guestNice;
    ss >> tag >> u >> n >> s >> id >> io >> ir >> si >> steal >> guest >> guestNice;

    setTicks(u, n, s, id, io, ir, si);
    calculateUsage();

    this->lastUpdated = time(nullptr);   // [this POINTER] update protected member
}

// [OVERRIDE] returns human-readable CPU percentage string
std::string CpuInfo::toDisplayString() const {
    return "CPU: " + formatDouble(this->usagePercent) + "%";
}

// [GETTER] CPU usage as a double in [0.0, 100.0]
double CpuInfo::getUsagePercent() const {
    return this->usagePercent;
}

// [GETTER] total ticks (active + idle) in the current snapshot
unsigned long long CpuInfo::getTotalTicks() const {
    return user + nice + system + idle + iowait + irq + softirq;
}

// [GETTER] idle ticks in the current snapshot
unsigned long long CpuInfo::getIdleTicks() const {
    return idle;
}

// Stores new raw tick values, saving the previous snapshot first
void CpuInfo::setTicks(unsigned long long u, unsigned long long n,
                       unsigned long long s, unsigned long long id,
                       unsigned long long io, unsigned long long ir,
                       unsigned long long si)
{
    // Save current as previous before overwriting
    prevUser    = user;
    prevNice    = nice;
    prevSystem  = system;
    prevIdle    = idle;
    prevIowait  = iowait;
    prevIrq     = irq;
    prevSoftirq = softirq;

    // Store the new snapshot
    user    = u;
    nice    = n;
    system  = s;
    idle    = id;
    iowait  = io;
    irq     = ir;
    softirq = si;
}

// [TEMPLATE FUNCTION USE] calculatePercent<unsigned long long> from Formatter.h
// Recalculates usagePercent from the delta between current and previous snapshots
void CpuInfo::calculateUsage() {
    unsigned long long prevTotal = prevUser + prevNice + prevSystem + prevIdle
                                   + prevIowait + prevIrq + prevSoftirq;
    unsigned long long currTotal = user + nice + system + idle
                                   + iowait + irq + softirq;

    unsigned long long totalDelta = currTotal - prevTotal;
    unsigned long long idleDelta  = idle - prevIdle;

    // [TEMPLATE] calculatePercent<unsigned long long> — generic percentage helper
    this->usagePercent = calculatePercent<unsigned long long>(
        totalDelta - idleDelta, totalDelta);

    // [TEMPLATE] clamp result to valid range [0.0, 100.0]
    this->usagePercent = clamp<double>(this->usagePercent, 0.0, 100.0);
}
