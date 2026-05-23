/*
 * MonitorExceptions.h
 * ===================
 * Description: Custom exception class hierarchy for the Linux Monitor application.
 *              Provides specific exception types for file read failures and parse errors.
 *
 * OOP Concepts Demonstrated:
 *   - Inheritance (exception hierarchy)
 *   - Abstract/Base class with virtual functions
 *   - Constructor chaining
 *   - Encapsulation
 *
 * Lecture Reference: Lecture 11 (Exception Handling)
 *
 * Author: OOP 2 Project Team
 * Course: OOP 2 (MSC1052) — Spring 2026
 */

#pragma once

#include <exception>
#include <string>

/**
 * @class MonitorException
 * @brief Base exception class for all Linux Monitor application errors.
 *
 * Demonstrates: inheritance from std::exception, virtual functions,
 * encapsulation of error message.
 *
 * Corresponds to: Lecture 11
 */
// [ABSTRACT CLASS] Base exception — inherits from std::exception
class MonitorException : public std::exception {
protected:
    std::string message;   // [ENCAPSULATION] protected — accessible by derived classes

public:
    // [PARAMETERIZED CONSTRUCTOR] initialize with error message
    explicit MonitorException(const std::string& msg) : message(msg) {}

    // [VIRTUAL FUNCTION] override what() for polymorphic error reporting
    const char* what() const noexcept override { return message.c_str(); }

    // [VIRTUAL DESTRUCTOR] mandatory for polymorphic base class
    virtual ~MonitorException() {}
};

/**
 * @class FileReadException
 * @brief Thrown when a /proc file cannot be opened or read.
 *
 * Demonstrates: inheritance, constructor chaining, encapsulation.
 * Corresponds to: Lecture 11
 */
// [INHERITANCE] FileReadException IS-A MonitorException
class FileReadException : public MonitorException {
private:
    std::string filePath;   // [ENCAPSULATION] path to the file that failed

public:
    // [CONSTRUCTOR CHAINING] calls MonitorException constructor
    explicit FileReadException(const std::string& path)
        : MonitorException("Cannot read file: " + path), filePath(path) {}

    // [GETTER] returns the problematic file path
    std::string getFilePath() const { return filePath; }

    ~FileReadException() override {}
};

/**
 * @class ParseException
 * @brief Thrown when /proc file content cannot be parsed as expected.
 *
 * Demonstrates: inheritance, constructor chaining.
 * Corresponds to: Lecture 11
 */
// [INHERITANCE] ParseException IS-A MonitorException
class ParseException : public MonitorException {
public:
    // [CONSTRUCTOR CHAINING] delegates to MonitorException
    explicit ParseException(const std::string& msg)
        : MonitorException("Parse error: " + msg) {}

    ~ParseException() override {}
};

/**
 * @class ProcessNotFoundException
 * @brief Thrown when a requested process PID no longer exists.
 *
 * Demonstrates: inheritance, encapsulation, constructor chaining.
 * Corresponds to: Lecture 11
 */
// [INHERITANCE] ProcessNotFoundException IS-A MonitorException
class ProcessNotFoundException : public MonitorException {
private:
    int pid;   // [ENCAPSULATION] the missing PID

public:
    // [CONSTRUCTOR CHAINING] passes formatted message to base
    explicit ProcessNotFoundException(int p)
        : MonitorException("Process not found: PID " + std::to_string(p)), pid(p) {}

    // [GETTER] accessor for the PID
    int getPid() const { return pid; }

    ~ProcessNotFoundException() override {}
};
