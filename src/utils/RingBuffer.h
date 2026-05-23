/*
 * RingBuffer.h
 * ============
 * Description: Generic class template implementing a fixed-capacity circular buffer.
 *              Older entries are overwritten when the buffer is full.
 *              Used by CpuGraphWidget to store the last 60 CPU readings.
 *
 * OOP Concepts Demonstrated:
 *   - Class template (template <typename T>)
 *   - Encapsulation (private storage, capacity, head, count)
 *   - Operator overloading (operator[])
 *   - Default and parameterized constructors
 *   - Destructor
 *
 * Lecture Reference: Lecture 9 (Class Templates)
 *
 * Author: OOP 2 Project Team
 * Course: OOP 2 (MSC1052) — Spring 2026
 */

#pragma once

#include <vector>
#include <stdexcept>

/**
 * @class RingBuffer
 * @brief Fixed-size circular buffer that stores the last N elements of type T.
 *
 * Demonstrates: class template, operator[], encapsulation.
 * Corresponds to: Lecture 9
 */
// [CLASS TEMPLATE] generic circular buffer — works with any type T
template <typename T>
class RingBuffer {
private:
    // [ENCAPSULATION] all storage is private
    std::vector<T> buffer;   // underlying storage
    int capacity;             // maximum number of elements
    int head;                 // index where the next push will write
    int count;                // how many valid elements are currently stored

public:
    // [PARAMETERIZED CONSTRUCTOR] must specify capacity at construction time
    explicit RingBuffer(int cap)
        : capacity(cap), head(0), count(0)
    {
        buffer.resize(static_cast<std::size_t>(capacity));
    }

    // [DEFAULT CONSTRUCTOR] capacity = 60 (60 seconds of data by default)
    RingBuffer() : RingBuffer(60) {}

    // [DESTRUCTOR] vector handles its own memory — nothing extra needed
    ~RingBuffer() {}

    /**
     * @brief Pushes a new value into the buffer.
     * If full, the oldest entry is silently overwritten.
     */
    void push(const T& value) {
        buffer[static_cast<std::size_t>(head)] = value;
        head = (head + 1) % capacity;
        if (count < capacity) ++count;
    }

    /**
     * @brief Index access — index 0 is the oldest element, index count-1 is newest.
     * [OPERATOR OVERLOAD] operator[] with bounds check
     */
    T& operator[](int index) {
        if (index < 0 || index >= count) {
            throw std::out_of_range("RingBuffer index out of range");
        }
        int realIndex = (head - count + index + capacity) % capacity;
        return buffer[static_cast<std::size_t>(realIndex)];
    }

    /**
     * @brief Const index access.
     * [OPERATOR OVERLOAD] const operator[]
     */
    const T& operator[](int index) const {
        if (index < 0 || index >= count) {
            throw std::out_of_range("RingBuffer index out of range");
        }
        int realIndex = (head - count + index + capacity) % capacity;
        return buffer[static_cast<std::size_t>(realIndex)];
    }

    // [GETTER] number of valid elements currently stored
    int size() const { return count; }

    // [GETTER] maximum capacity
    int getCapacity() const { return capacity; }

    // [GETTER] true when buffer holds exactly capacity elements
    bool isFull() const { return count == capacity; }

    // Resets the buffer to empty (does not resize)
    void clear() {
        head  = 0;
        count = 0;
    }
};
