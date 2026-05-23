/*
 * Formatter.h
 * ===========
 * Description: Function templates and overloaded formatting helpers.
 *              Provides clamp<T>, calculatePercent<T>, findIndex<T>, and
 *              overloaded formatValue() for compile-time polymorphism.
 *
 * OOP Concepts Demonstrated:
 *   - Function templates (clamp, calculatePercent, findIndex)
 *   - Compile-time polymorphism via function overloading (formatValue)
 *   - Template specialization concept
 *
 * Lecture Reference: Lecture 9 (Templates), Lecture 7 (Polymorphism)
 *
 * Author: OOP 2 Project Team
 * Course: OOP 2 (MSC1052) — Spring 2026
 */

#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cmath>

// ============================================================================
// FUNCTION TEMPLATES — Lecture 9
// ============================================================================

/**
 * @brief Clamps value into [minVal, maxVal]. Works with any comparable type.
 * [FUNCTION TEMPLATE] compile-time polymorphism
 */
template <typename T>
T clamp(T value, T minVal, T maxVal) {
    if (value < minVal) return minVal;
    if (value > maxVal) return maxVal;
    return value;
}

/**
 * @brief Calculates what percentage 'part' is of 'total'.
 * Returns 0.0 if total is zero to avoid division by zero.
 * [FUNCTION TEMPLATE] works with any numeric type
 */
template <typename T>
double calculatePercent(T part, T total) {
    if (total == static_cast<T>(0)) return 0.0;
    return (static_cast<double>(part) / static_cast<double>(total)) * 100.0;
}

/**
 * @brief Searches a vector for target using operator== on T.
 * Returns the index, or -1 if not found.
 * [FUNCTION TEMPLATE] requires T to have operator==
 */
template <typename T>
int findIndex(const std::vector<T>& vec, const T& target) {
    for (int i = 0; i < static_cast<int>(vec.size()); ++i) {
        if (vec[i] == target) return i;   // [OPERATOR OVERLOAD USE] operator==
    }
    return -1;
}

// ============================================================================
// OVERLOADED FORMAT FUNCTIONS — compile-time polymorphism (Lecture 7)
// ============================================================================

/**
 * @brief Formats a double to 2 decimal places.
 * [COMPILE-TIME POLYMORPHISM] overloaded formatValue for double
 */
inline std::string formatValue(double value) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << value;
    return oss.str();
}

/**
 * @brief Formats a long (KB value) with thousands separators.
 * [COMPILE-TIME POLYMORPHISM] overloaded formatValue for long
 */
inline std::string formatValue(long valueKb) {
    std::string s = std::to_string(valueKb);
    // Insert commas every 3 digits from the right
    int insertPos = static_cast<int>(s.size()) - 3;
    while (insertPos > 0) {
        s.insert(static_cast<std::size_t>(insertPos), ",");
        insertPos -= 3;
    }
    return s + " KB";
}

/**
 * @brief Formats an int (PID) as "PID: XXXX".
 * [COMPILE-TIME POLYMORPHISM] overloaded formatValue for int
 */
inline std::string formatValue(int pid) {
    return "PID: " + std::to_string(pid);
}

/**
 * @brief Formats a double as a percentage string (e.g. "42.30%").
 * Convenience wrapper used by CpuInfo::toDisplayString().
 */
inline std::string formatDouble(double value) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << value;
    return oss.str();
}

/**
 * @brief Formats memory in KB to a readable string (MB if >= 1024).
 */
inline std::string formatMemory(long kb) {
    if (kb >= 1024) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(1)
            << (static_cast<double>(kb) / 1024.0) << " MB";
        return oss.str();
    }
    return std::to_string(kb) + " KB";
}
