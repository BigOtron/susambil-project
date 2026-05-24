/*
 * Formatter.h
 * ===========
 * General-purpose template functions and overloaded helpers for formatting.
 *
 * OOP Concepts:
 *   - Function templates (clamp, calculatePercent, findIndex)
 *   - Compile-time polymorphism via function overloading (formatValue)
 *
 * Author: OOP 2 Project Team
 * Course: OOP 2 — Spring 2026
 */

#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cmath>

using namespace std;

// ─────────────────────────────────────────────
// TEMPLATE FUNCTIONS
// ─────────────────────────────────────────────

// Keeps a value within [minVal, maxVal] range.
// Example: clamp(150, 0, 100) returns 100
template <typename T>
T clamp(T value, T minVal, T maxVal) {
    if (value < minVal) return minVal;
    if (value > maxVal) return maxVal;
    return value;
}

// Calculates what percentage 'part' is out of 'total'.
// Returns 0.0 when total is zero to avoid division by zero.
template <typename T>
double calculatePercent(T part, T total) {
    if (total == static_cast<T>(0)) return 0.0;
    return (static_cast<double>(part) / static_cast<double>(total)) * 100.0;
}

// Searches a vector for a target value and returns its index.
// Returns -1 if the target is not found.
template <typename T>
int findIndex(const vector<T>& vec, const T& target) {
    for (int i = 0; i < static_cast<int>(vec.size()); ++i) {
        if (vec[i] == target) return i;
    }
    return -1;
}

// ─────────────────────────────────────────────
// FORMAT FUNCTIONS (overloading)
// ─────────────────────────────────────────────

// Formats a double to 2 decimal places. Example: 3.14159 → "3.14"
inline string formatValue(double value) {
    ostringstream oss;
    oss << fixed << setprecision(2) << value;
    return oss.str();
}

// Formats a long (KB value) with thousands separators. Example: 1024 → "1,024 KB"
inline string formatValue(long valueKb) {
    string s = to_string(valueKb);
    // Insert a comma every 3 digits from the right
    int insertPos = static_cast<int>(s.size()) - 3;
    while (insertPos > 0) {
        s.insert(static_cast<size_t>(insertPos), ",");
        insertPos -= 3;
    }
    return s + " KB";
}

// Formats an int as a PID label. Example: 1234 → "PID: 1234"
inline string formatValue(int pid) {
    return "PID: " + to_string(pid);
}

// Formats a double as a percentage string with 1 decimal. Example: 42.3 → "42.3"
inline string formatDouble(double value) {
    ostringstream oss;
    oss << fixed << setprecision(1) << value;
    return oss.str();
}

// Converts KB to a readable string. Shows MB if value is 1024 KB or more.
// Example: 2048 → "2.0 MB", 512 → "512 KB"
inline string formatMemory(long kb) {
    if (kb >= 1024) {
        ostringstream oss;
        oss << fixed << setprecision(1)
            << (static_cast<double>(kb) / 1024.0) << " MB";
        return oss.str();
    }
    return to_string(kb) + " KB";
}