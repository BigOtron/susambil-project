/*
* IRenderable.h
 * =============
 * A simple interface that enforces one rule: any class that implements it
 * must be able to describe itself as a string.
 *
 * Nothing complex here — just a contract. If your class claims to be
 * IRenderable, it has to provide a toDisplayString() method. That's it.
 *
 * Author: OOP 2 Project Team
 * Course: OOP 2 (MSC1052) — Spring 2026
 */

#pragma once

#include <string>

/**
 * Any class that wants to produce a display string should implement this.
 *
 * Think of it as a promise — if you inherit from IRenderable, you're
 * promising that your class knows how to represent itself as text.
 */
// [ABSTRACT CLASS] Pure abstract class — you can't create an IRenderable directly, only inherit from it
class IRenderable {
public:
    // [VIRTUAL DESTRUCTOR] Needs to be virtual so that when you delete through a base pointer,
    // the right destructor gets called — standard rule for any polymorphic base class
    virtual ~IRenderable() = 0;

    // [PURE VIRTUAL] Every subclass must implement this — return whatever makes sense as a
    // human-readable summary of that object
    virtual std::string toDisplayString() const = 0;
};

// [ABSTRACT CLASS] Pure virtual destructors are a bit unusual — C++ requires you to provide
// a body for them even though they're pure virtual, so here it is
inline IRenderable::~IRenderable() {}
