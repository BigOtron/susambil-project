/*
 * Logger.h
 * ========
 * Description: Singleton logger class that writes timestamped messages to a log file.
 *              Demonstrates static members, static factory method, and file output.
 *
 * OOP Concepts Demonstrated:
 *   - Static members (instance pointer, message counter)
 *   - Static factory method (getInstance)
 *   - Encapsulation (private constructor — singleton pattern)
 *   - File I/O (ofstream)
 *   - Destructor (closes log file)
 *
 * Lecture Reference: Lecture 3 (Static Members), Lecture 10 (File I/O)
 *
 * Author: OOP 2 Project Team
 * Course: OOP 2 (MSC1052) — Spring 2026
 */

#pragma once

#include <string>
#include <fstream>

/**
 * @class Logger
 * @brief Singleton logger that writes to a file and counts messages.
 *
 * Demonstrates: static members, static factory, encapsulation, file output.
 * Corresponds to: Lectures 3, 10
 */
class Logger {
private:
    // [STATIC MEMBER] single shared instance (singleton pattern)
    static Logger* instance;

    // [STATIC MEMBER] total messages logged — shared across all code paths
    static int messageCount;

    // [ENCAPSULATION] file stream — private, managed only by this class
    std::ofstream logFile;

    // [ENCAPSULATION] private constructor — prevents direct instantiation
    Logger();

public:
    // [STATIC MEMBER] factory method — creates or returns the singleton instance
    static Logger* getInstance();

    // [STATIC MEMBER] returns the total number of messages logged so far
    static int getMessageCount();

    // Log a message at a given severity level
    void log(const std::string& level, const std::string& message);

    void logInfo   (const std::string& msg);
    void logError  (const std::string& msg);
    void logWarning(const std::string& msg);

    // [DESTRUCTOR] closes the log file on program exit
    ~Logger();

    // Prevent copy and assignment — singleton must not be duplicated
    Logger(const Logger&)            = delete;
    Logger& operator=(const Logger&) = delete;
};
