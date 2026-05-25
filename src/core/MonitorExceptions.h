/*
* MonitorExceptions.h
 * ===================
 * Defines all the custom exceptions used across the Linux Monitor app.
 * Instead of throwing a generic std::exception everywhere, we have specific
 * types for each failure scenario — file read errors, parse errors, missing processes.
 * That way the caller knows exactly what went wrong and can handle it accordingly.
 *
 * Author: OOP 2 Project Team
 * Course: OOP 2 (MSC1052) — Spring 2026
 */


#pragma once

#include <exception>
#include <string>

/**
 * The root exception for everything in this application.
 *
 * All other exceptions in this file inherit from this one, so you can
 * catch just this type if you want to handle any monitor-related error
 * in one place, or catch the specific subclass if you need finer control.
 */
// [ABSTRACT CLASS] Base class — sits at the top of our exception hierarchy, inherits from std::exception
class MonitorException : public std::exception {
protected:
    std::string message;   // [ENCAPSULATION] kept protected so subclasses can read it if needed

public:
    // [PARAMETERIZED CONSTRUCTOR] Takes the error message and stores it — subclasses call this through chaining
    explicit MonitorException(const std::string& msg) : message(msg) {}

    // [VIRTUAL FUNCTION] what() is the standard way to get an error message out of any exception
    // we override it here to return our custom message instead of something generic
    const char* what() const noexcept override { return message.c_str(); }

    // [VIRTUAL DESTRUCTOR] mandatory for polymorphic base class
    // without it, deleting through a base pointer would skip the derived destructor
    virtual ~MonitorException() {}
};

/**
 * Thrown when we try to open a /proc file and it fails.
 *
 * Stores the path that caused the problem so the error message is actually useful
 * — "cannot read file: /proc/1234/status" beats a generic "file error".
 */
// [INHERITANCE] Specific to file I/O failures — extends MonitorExceptionn
class FileReadException : public MonitorException {
private:
    std::string filePath;   // [ENCAPSULATION] the exact path that couldn't be opened

public:
    // [CONSTRUCTOR CHAINING] Builds a readable message automatically and passes it up to the base class
    explicit FileReadException(const std::string& path)
        : MonitorException("Cannot read file: " + path), filePath(path) {}

    // [GETTER] In case the caller wants to know which file failed — e.g. for logging
    std::string getFilePath() const { return filePath; }

    ~FileReadException() override {}
};

/**
 * Thrown when a /proc file opens fine but the content isn't what we expected.
 *
 * A separate type from FileReadException because the cause is different —
 * the file exists, we just couldn't make sense of what's inside it.
 */
// [INHERITANCE] For when the file is readable but the data inside is malformed or unexpected
class ParseException : public MonitorException {
public:
    // [CONSTRUCTOR CHAINING] Prepends "Parse error:" to whatever context message is passed in
    explicit ParseException(const std::string& msg)
        : MonitorException("Parse error: " + msg) {}

    ~ParseException() override {}
};

/**
 * Thrown when we look up a PID that no longer exists.
 *
 * Processes can die at any moment, so this will happen in normal usage —
 * not just on bugs. The PID is stored separately so callers can log or
 * display it without having to parse the message string.
 */
// [INHERITANCE] For when a process disappears between when we listed it and when we tried to access it
class ProcessNotFoundException : public MonitorException {
private:
    int pid;   // [ENCAPSULATION] the PID we were looking for

public:
    // [CONSTRUCTOR CHAINING] Formats a clear message like "Process not found: PID 4821" and stores the PID
    explicit ProcessNotFoundException(int p)
        : MonitorException("Process not found: PID " + std::to_string(p)), pid(p) {}

    // [GETTER] Returns the PID directly — useful if the caller wants to handle it without string parsing
    int getPid() const { return pid; }

    ~ProcessNotFoundException() override {}
};
