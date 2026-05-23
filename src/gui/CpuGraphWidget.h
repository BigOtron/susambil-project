/*
 * CpuGraphWidget.h
 * ================
 * Description: Custom QWidget that draws a live scrolling CPU usage graph using
 *              QPainter. Stores history in a RingBuffer<double> class template.
 *
 * OOP Concepts Demonstrated:
 *   - Inheritance (derives from QWidget)
 *   - Composition (has-a RingBuffer<double>)
 *   - Class template usage (RingBuffer<double>)
 *   - Override of paintEvent (runtime polymorphism via Qt)
 *   - Encapsulation
 *
 * Lecture Reference: Lectures 9 (Templates), 12-14 (Qt GUI, custom paint)
 *
 * Author: OOP 2 Project Team
 * Course: OOP 2 (MSC1052) — Spring 2026
 */

#pragma once

#include <QWidget>
#include <QColor>
#include "../utils/RingBuffer.h"

/**
 * @class CpuGraphWidget
 * @brief Custom widget that draws a scrolling CPU usage history graph.
 *
 * Demonstrates: Qt custom painting, RingBuffer<double> template usage,
 * inheritance from QWidget, paintEvent override.
 *
 * Corresponds to: Lectures 9, 12-14
 */
// [INHERITANCE] CpuGraphWidget IS-A QWidget
class CpuGraphWidget : public QWidget {
    Q_OBJECT   // [Qt] required for signals/slots

private:
    // [COMPOSITION] HAS-A RingBuffer<double> — stores last 60 CPU readings
    // [CLASS TEMPLATE] RingBuffer instantiated with type double
    RingBuffer<double> history;

    QColor graphColor;        // color of the graph line
    QColor fillColor;         // semi-transparent fill under the line
    QColor gridColor;         // grid line color
    QColor backgroundColor;   // widget background

public:
    // [PARAMETERIZED CONSTRUCTOR]
    explicit CpuGraphWidget(QWidget* parent = nullptr);

    // [DESTRUCTOR]
    ~CpuGraphWidget() override;

    /**
     * @brief Appends a new CPU reading to the ring buffer and triggers a repaint.
     */
    void addDataPoint(double value);

    /**
     * @brief Returns the most recent CPU percentage stored in the buffer.
     */
    double currentValue() const;

    // [Qt] preferred size hint for layout managers
    QSize sizeHint() const override;

protected:
    // [OVERRIDE] called by Qt whenever the widget needs to be redrawn
    // [POLYMORPHISM] virtual function override — runtime dispatch from QWidget
    void paintEvent(QPaintEvent* event) override;

private:
    // Private drawing helpers — called from paintEvent
    void drawBackground(QPainter& painter);
    void drawGrid(QPainter& painter);
    void drawGraph(QPainter& painter);
    void drawLabels(QPainter& painter);
    void drawCurrentValue(QPainter& painter);
};
