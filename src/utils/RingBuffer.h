/*
 * RingBuffer.h
 * ============
 * A generic fixed-size circular buffer. When full, new values
 * overwrite the oldest ones. Used by CpuGraphWidget to keep
 * the last 60 CPU readings.
 *
 * OOP Concepts:
 *   - Class template (template <typename T>)
 *   - Encapsulation (private storage, capacity, head, count)
 *   - Operator overloading (operator[])
 *   - Default and parameterized constructors
 *   - Destructor
 *
 * Author: OOP 2 Project Team
 * Course: OOP 2  — Spring 2026
 */

#pragma once

#include <vector>
#include <stdexcept>

using namespace std;

// Works with any type T — int, double, string, whatever you pass in
template <typename T>
class RingBuffer {
private:
    vector<T> buffer;  // the actual storage array
    int capacity;      // max number of elements this buffer can hold
    int head;          // where the next push() will write
    int count;         // how many valid elements are in the buffer right now

public:
    // You must tell it how many elements to hold at construction time
    explicit RingBuffer(int cap)
        : capacity(cap), head(0), count(0)
    {
        buffer.resize(static_cast<size_t>(capacity));
    }

    // Default size is 60 — enough for 60 seconds of readings
    RingBuffer() : RingBuffer(60) {}

    // vector cleans up its own memory, nothing extra to do here
    ~RingBuffer() {}

    // Adds a new value. If the buffer is already full, the oldest value is overwritten.
    void push(const T& value) {
        buffer[static_cast<size_t>(head)] = value;
        head = (head + 1) % capacity;
        if (count < capacity) ++count;
    }

    // index 0 = oldest element, index count-1 = newest element.
    // Throws if index is out of range.
    T& operator[](int index) {
        if (index < 0 || index >= count) {
            throw out_of_range("RingBuffer index out of range");
        }
        int realIndex = (head - count + index + capacity) % capacity;
        return buffer[static_cast<size_t>(realIndex)];
    }

    // Same as above but for read-only access (const contexts)
    const T& operator[](int index) const {
        if (index < 0 || index >= count) {
            throw out_of_range("RingBuffer index out of range");
        }
        int realIndex = (head - count + index + capacity) % capacity;
        return buffer[static_cast<size_t>(realIndex)];
    }

    // How many elements are currently stored
    int size() const { return count; }

    // The maximum number of elements this buffer can hold
    int getCapacity() const { return capacity; }

    // True when the buffer has no free slots left
    bool isFull() const { return count == capacity; }

    // Empties the buffer without changing its capacity
    void clear() {
        head  = 0;
        count = 0;
    }
};