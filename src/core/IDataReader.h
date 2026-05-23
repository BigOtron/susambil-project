/*
 * IDataReader.h
 * =============
 * Description: Pure abstract interface for all data reader classes.
 *              Defines the contract that CpuReader and ProcessReader must fulfill.
 *
 * OOP Concepts Demonstrated:
 *   - Abstract class (cannot be instantiated)
 *   - Pure virtual functions (= 0)
 *   - Interface pattern
 *   - Virtual destructor
 *
 * Lecture Reference: Lecture 9 (Abstract Classes & Templates)
 *
 * Author: OOP 2 Project Team
 * Course: OOP 2 (MSC1052) — Spring 2026
 */

#pragma once

/**
 * @class IDataReader
 * @brief Pure abstract interface for reading system data from /proc filesystem.
 *
 * Demonstrates: abstract class, pure virtual functions, interface pattern.
 * Any class implementing this interface MUST override all pure virtual methods.
 *
 * Corresponds to: Lecture 9
 */
// [ABSTRACT CLASS] IDataReader — cannot be instantiated directly
class IDataReader {
public:
    // [VIRTUAL DESTRUCTOR] pure virtual destructor — still needs a definition
    virtual ~IDataReader() = 0;

    // [PURE VIRTUAL] open the /proc source file or directory
    virtual bool openSource() = 0;

    // [PURE VIRTUAL] read and parse data from the source
    virtual void readData() = 0;

    // [PURE VIRTUAL] close the file handle/resource
    virtual void closeSource() = 0;

    // [PURE VIRTUAL] check if the data source is available on this system
    virtual bool isAvailable() const = 0;
};

// [ABSTRACT CLASS] pure virtual destructor must be defined outside the class
inline IDataReader::~IDataReader() {}
