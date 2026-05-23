/*
 * ProcessList.cpp
 * ===============
 * Description: Implementation of ProcessList — container for ProcessInfo objects.
 *              Shows static member initialization, STL algorithm usage, and
 *              operator[] overload.
 *
 * OOP Concepts Demonstrated:
 *   - Static member initialization and access
 *   - STL vector and <algorithm> (sort, find_if)
 *   - Operator overloading (operator[])
 *   - Composition
 *
 * Lecture Reference: Lecture 3 (Static Members), Lecture 4-5 (Operators)
 *
 * Author: OOP 2 Project Team
 * Course: OOP 2 (MSC1052) — Spring 2026
 */

#include "ProcessList.h"
#include "../utils/Logger.h"
#include <algorithm>
#include <stdexcept>

// [STATIC MEMBER] definition outside the class — required for non-inline static
int ProcessList::totalCreated = 0;

// [DEFAULT CONSTRUCTOR] increments the static counter on each instance creation
ProcessList::ProcessList() {
    // [STATIC MEMBER] increment shared counter
    ++ProcessList::totalCreated;
    Logger::getInstance()->logInfo("ProcessList created (total: "
                                   + std::to_string(ProcessList::totalCreated) + ")");
}

// [DESTRUCTOR] logs cleanup
ProcessList::~ProcessList() {
    Logger::getInstance()->logInfo("ProcessList destroyed");
}

// Appends a copy of the given process to the internal vector
void ProcessList::add(const ProcessInfo& p) {
    processes.push_back(p);   // push_back copies p (uses copy constructor)
}

// Removes all process entries from the vector
void ProcessList::clear() {
    processes.clear();
}

// Returns the number of processes currently held
int ProcessList::count() const {
    return static_cast<int>(processes.size());
}

// [CONST REFERENCE] returns internal vector without copying it
const std::vector<ProcessInfo>& ProcessList::getAll() const {
    return this->processes;
}

// Sort descending by CPU usage — uses operator> defined on ProcessInfo
void ProcessList::sortByCpu() {
    // [OPERATOR OVERLOAD USE] lambda uses operator> on ProcessInfo
    std::sort(processes.begin(), processes.end(),
              [](const ProcessInfo& a, const ProcessInfo& b) {
                  return a > b;   // [OPERATOR OVERLOAD] operator> — descending CPU
              });
}

// Sort descending by resident memory
void ProcessList::sortByMemory() {
    std::sort(processes.begin(), processes.end(),
              [](const ProcessInfo& a, const ProcessInfo& b) {
                  return a.getMemoryKb() > b.getMemoryKb();
              });
}

// Sort ascending by PID
void ProcessList::sortByPid() {
    std::sort(processes.begin(), processes.end(),
              [](const ProcessInfo& a, const ProcessInfo& b) {
                  return a.getPid() < b.getPid();
              });
}

// Sort ascending by process name
void ProcessList::sortByName() {
    std::sort(processes.begin(), processes.end(),
              [](const ProcessInfo& a, const ProcessInfo& b) {
                  return a.getName() < b.getName();
              });
}

// Finds the process matching the given PID — uses operator== on ProcessInfo
// Returns pointer into the vector (nullptr if not found)
ProcessInfo* ProcessList::findByPid(int pid) {
    // [OPERATOR OVERLOAD USE] operator== is used inside findIndex<T>
    for (auto& p : processes) {
        if (p.getPid() == pid) {   // [OPERATOR OVERLOAD] operator==
            return &p;
        }
    }
    return nullptr;
}

// [OPERATOR OVERLOAD] index access with bounds check
ProcessInfo& ProcessList::operator[](int index) {
    if (index < 0 || index >= static_cast<int>(processes.size())) {
        throw std::out_of_range("ProcessList index out of range: " + std::to_string(index));
    }
    return processes[static_cast<std::size_t>(index)];
}

// [OPERATOR OVERLOAD] const index access
const ProcessInfo& ProcessList::operator[](int index) const {
    if (index < 0 || index >= static_cast<int>(processes.size())) {
        throw std::out_of_range("ProcessList index out of range: " + std::to_string(index));
    }
    return processes[static_cast<std::size_t>(index)];
}

// [STATIC MEMBER] returns the shared creation counter
int ProcessList::getTotalCreated() {
    return ProcessList::totalCreated;
}
