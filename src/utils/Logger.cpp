/*
 * Logger.cpp
 * ==========
 * Implementation of the Logger singleton.
 * Opens monitor.log on first use, writes timestamped messages,
 * and keeps track of how many messages have been logged.
 *
 * OOP Concepts:
 *   - Static member initialization and lazy initialization
 *   - Private constructor (singleton pattern)
 *   - Static factory method
 *   - File I/O with ofstream
 *   - Destructor for clean file close
 *
 * Author: OOP 2 Project Team
 * Course: OOP 2 — Spring 2026
 */

#include "Logger.h"
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>

using namespace std;

// Static members must be defined in exactly one .cpp file
Logger* Logger::instance     = nullptr;
int     Logger::messageCount = 0;

// Private constructor — only getInstance() can call this
Logger::Logger() {
    // Open in append mode so logs from previous runs are not lost
    logFile.open("monitor.log", ios::out | ios::app);
    if (!logFile.is_open()) {
        cerr << "[Logger] WARNING: cannot open monitor.log\n";
    }
    log("INFO", "=== Logger initialized ===");
}

// Destructor — closes the file when the program exits
Logger::~Logger() {
    if (logFile.is_open()) {
        log("INFO", "=== Logger shutting down ===");
        logFile.close();
    }
    // We don't delete instance here — that would cause recursion.
    // The OS reclaims memory on exit anyway.
}

// Returns the single Logger instance, creating it on the very first call
Logger* Logger::getInstance() {
    if (instance == nullptr) {
        instance = new Logger();
    }
    return instance;
}

// Returns how many messages have been logged so far
int Logger::getMessageCount() {
    return messageCount;
}

// Builds a timestamped log line and writes it to the file.
// ERROR level messages are also printed to the terminal.
void Logger::log(const string& level, const string& message) {
    // Build a readable timestamp like "2026-05-24 13:45:00"
    time_t now = time(nullptr);
    tm* tm_info = localtime(&now);
    ostringstream ts;
    ts << put_time(tm_info, "%Y-%m-%d %H:%M:%S");

    string line = "[" + ts.str() + "] [" + level + "] " + message;

    // Write to the log file and flush immediately so nothing is lost on crash
    if (logFile.is_open()) {
        logFile << line << "\n";
        logFile.flush();
    }

    // Errors also go to stderr so the developer sees them in the terminal
    if (level == "ERROR") {
        cerr << line << "\n";
    }

    ++messageCount;
}

// The three public helpers just forward to log() with the right level tag
void Logger::logInfo(const string& msg) {
    log("INFO", msg);
}

void Logger::logError(const string& msg) {
    log("ERROR", msg);
}

void Logger::logWarning(const string& msg) {
    log("WARNING", msg);
}