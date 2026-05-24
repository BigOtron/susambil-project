/*
* Logger.h
 * ========
 * Singleton logger class that writes timestamped messages to a log file.
 *
 * OOP Concepts:
 *   - Static members (instance pointer, message counter)
 *   - Static factory method (getInstance)
 *   - Encapsulation via private constructor (singleton pattern)
 *   - File I/O with ofstream
 *   - Destructor for clean file close
 *
 * Author: OOP 2 Project Team
 * Course: OOP 2  — Spring 2026
 */

#pragma once

#include <string>
#include <fstream>

using namespace std;

class Logger {
private:
    // Only one Logger can exist — this holds that single instance
    static Logger* instance;

    // Counts every message logged across the whole program
    static int messageCount;

    // The actual file we write logs into — only this class touches it
    ofstream logFile;

    // Private so nothing outside this class can create a Logger directly
    Logger();

public:
    // Returns the one shared Logger, creating it on the first call
    static Logger* getInstance();

    // Returns how many messages have been logged so far
    static int getMessageCount();

    // Writes a message with a severity level (INFO, ERROR, WARNING)
    void log(const string& level, const string& message);

    void logInfo   (const string& msg);
    void logError  (const string& msg);
    void logWarning(const string& msg);

    // Closes the log file cleanly when the program exits
    ~Logger();

    // Copying a singleton makes no sense — both deleted to prevent accidents
    Logger(const Logger&)            = delete;
    Logger& operator=(const Logger&) = delete;
};