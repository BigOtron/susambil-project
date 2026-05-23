/*
 * Logger.cpp
 * ==========
 * Description: Implementation of the Logger singleton.
 *              Opens monitor.log on first getInstance() call, appends timestamped
 *              messages, and tracks a static message count.
 *
 * OOP Concepts Demonstrated:
 *   - Static member initialization and lazy initialization
 *   - Private constructor (singleton)
 *   - Static factory method
 *   - File output (ofstream)
 *   - Destructor (file close)
 *
 * Lecture Reference: Lecture 3 (Static Members), Lecture 10 (File I/O)
 *
 * Author: OOP 2 Project Team
 * Course: OOP 2 (MSC1052) — Spring 2026
 */

#include "Logger.h"
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>

// [STATIC MEMBER] definitions — must live in exactly one .cpp file
Logger* Logger::instance     = nullptr;
int     Logger::messageCount = 0;

// [PRIVATE CONSTRUCTOR] called only by getInstance()
Logger::Logger() {
    // [FILE I/O] open log file in append mode so previous runs are preserved
    logFile.open("monitor.log", std::ios::out | std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "[Logger] WARNING: cannot open monitor.log\n";
    }
    log("INFO", "=== Logger initialized ===");
}

// [DESTRUCTOR] closes the log file cleanly on program exit
Logger::~Logger() {
    if (logFile.is_open()) {
        log("INFO", "=== Logger shutting down ===");
        logFile.close();   // [FILE I/O] explicit close
    }
    // Do NOT delete instance here — it would recurse; the OS cleans up on exit.
}

// [STATIC MEMBER] lazy singleton factory — creates the instance on first call
Logger* Logger::getInstance() {
    if (instance == nullptr) {
        instance = new Logger();   // [STATIC MEMBER] stored in static pointer
    }
    return instance;
}

// [STATIC MEMBER] returns the total count of messages logged
int Logger::getMessageCount() {
    return messageCount;   // [STATIC MEMBER] access without an object
}

// Formats and writes a single log line to the file (and stderr for errors)
void Logger::log(const std::string& level, const std::string& message) {
    // Build a timestamp string
    std::time_t now = std::time(nullptr);
    std::tm* tm_info = std::localtime(&now);
    std::ostringstream ts;
    ts << std::put_time(tm_info, "%Y-%m-%d %H:%M:%S");

    std::string line = "[" + ts.str() + "] [" + level + "] " + message;

    // [FILE I/O] write to log file
    if (logFile.is_open()) {
        logFile << line << "\n";
        logFile.flush();   // ensure it reaches disk immediately
    }

    // Also print errors to stderr so they appear in the terminal
    if (level == "ERROR") {
        std::cerr << line << "\n";
    }

    // [STATIC MEMBER] increment the shared counter
    ++messageCount;
}

void Logger::logInfo(const std::string& msg) {
    log("INFO", msg);
}

void Logger::logError(const std::string& msg) {
    log("ERROR", msg);
}

void Logger::logWarning(const std::string& msg) {
    log("WARNING", msg);
}
