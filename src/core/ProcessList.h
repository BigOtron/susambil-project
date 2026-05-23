/*
 * ProcessList.h
 * =============
 * Description: Container class managing a collection of ProcessInfo objects.
 *              Demonstrates STL vector usage, static members, operator overloading,
 *              friend class relationship, and sorting via operator<.
 *
 * OOP Concepts Demonstrated:
 *   - Composition (has-a vector<ProcessInfo>)
 *   - Static member (totalCreated counter)
 *   - Operator overloading (operator[])
 *   - STL vector usage
 *   - Encapsulation
 *
 * Lecture Reference: Lecture 3 (Composition, Static), Lecture 4-5 (Operators)
 *
 * Author: OOP 2 Project Team
 * Course: OOP 2 (MSC1052) — Spring 2026
 */

#pragma once

#include "ProcessInfo.h"
#include <vector>
#include <string>

/**
 * @class ProcessList
 * @brief Manages a collection of ProcessInfo objects with sorting and lookup.
 *
 * Demonstrates: composition, static members, operator[], STL vector.
 * Corresponds to: Lectures 3, 4, 5
 */
class ProcessList {
private:
    // [COMPOSITION] ProcessList HAS-A vector of ProcessInfo objects
    std::vector<ProcessInfo> processes;

    // [STATIC MEMBER] shared counter — tracks total instances ever created
    static int totalCreated;

public:
    // [DEFAULT CONSTRUCTOR]
    ProcessList();

    // [DESTRUCTOR]
    ~ProcessList();

    // Add a process to the list
    void add(const ProcessInfo& p);

    // Remove all entries
    void clear();

    // Number of processes currently stored
    int count() const;

    // [CONST REFERENCE] avoids copying the entire vector
    const std::vector<ProcessInfo>& getAll() const;

    // Sort descending by CPU usage — uses operator> on ProcessInfo
    void sortByCpu();

    // Sort descending by memory
    void sortByMemory();

    // Sort ascending by PID
    void sortByPid();

    // Sort ascending by name
    void sortByName();

    // Find process by PID — uses operator== on ProcessInfo; returns nullptr if missing
    ProcessInfo* findByPid(int pid);

    // [OPERATOR OVERLOAD] index access — non-const version
    ProcessInfo& operator[](int index);

    // [OPERATOR OVERLOAD] index access — const version
    const ProcessInfo& operator[](int index) const;

    // [STATIC MEMBER] returns how many ProcessList instances have been created
    static int getTotalCreated();
};
