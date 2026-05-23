/*
 * IRenderable.h
 * =============
 * Description: Pure abstract interface for objects that can produce display strings.
 *              Implemented by any class that needs to render itself as text.
 *
 * OOP Concepts Demonstrated:
 *   - Abstract class (interface)
 *   - Pure virtual functions
 *   - Virtual destructor
 *
 * Lecture Reference: Lecture 9 (Abstract Classes & Templates)
 *
 * Author: OOP 2 Project Team
 * Course: OOP 2 (MSC1052) — Spring 2026
 */

#pragma once

#include <string>

/**
 * @class IRenderable
 * @brief Pure abstract interface for objects that can produce a display string.
 *
 * Demonstrates: interface (pure abstract class), pure virtual functions.
 * Corresponds to: Lecture 9
 */
// [ABSTRACT CLASS] IRenderable — pure interface, cannot be instantiated
class IRenderable {
public:
    // [VIRTUAL DESTRUCTOR] required for safe polymorphic deletion
    virtual ~IRenderable() = 0;

    // [PURE VIRTUAL] returns a human-readable string representation
    virtual std::string toDisplayString() const = 0;
};

// [ABSTRACT CLASS] definition of pure virtual destructor
inline IRenderable::~IRenderable() {}
