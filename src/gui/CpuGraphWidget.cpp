/*
 * CpuGraphWidget.cpp
 * ==================
 * Description: Draws a live scrolling CPU usage line graph using QPainter.
 *              History is stored in a RingBuffer<double> class template instance.
 *
 * OOP Concepts Demonstrated:
 *   - Custom paintEvent override (runtime polymorphism via Qt)
 *   - Class template usage: RingBuffer<double>
 *   - Composition (has-a RingBuffer)
 *   - Encapsulation (private drawing helpers)
 *   - Inheritance from QWidget
 *
 * Lecture Reference: Lectures 9 (Templates), 12-14 (Qt custom painting)
 *
 * Author: OOP 2 Project Team
 * Course: OOP 2 (MSC1052) — Spring 2026
 */

#include "CpuGraphWidget.h"
#include <QPainter>
#include <QPaintEvent>
#include <QLinearGradient>
#include <QPainterPath>

// [PARAMETERIZED CONSTRUCTOR] — initializes colors and a 60-sample ring buffer
// [CONSTRUCTOR CHAINING] QWidget base is initialized with parent
CpuGraphWidget::CpuGraphWidget(QWidget* parent)
    : QWidget(parent),
      history(60),                          // [CLASS TEMPLATE] RingBuffer<double>(60)
      graphColor(QColor(0, 162, 232)),       // bright blue graph line
      fillColor(QColor(0, 162, 232, 60)),    // translucent fill
      gridColor(QColor(60, 60, 80)),         // dim grid
      backgroundColor(QColor(18, 18, 28))   // dark background
{
    setMinimumHeight(160);
    setMinimumWidth(300);
}

// [DESTRUCTOR]
CpuGraphWidget::~CpuGraphWidget() {}

// Appends a CPU reading and schedules a repaint
void CpuGraphWidget::addDataPoint(double value) {
    history.push(value);   // [CLASS TEMPLATE] RingBuffer<double>::push
    update();              // [Qt] schedules paintEvent call
}

// Returns the latest CPU reading (or 0 if buffer is empty)
double CpuGraphWidget::currentValue() const {
    if (history.size() == 0) return 0.0;
    return history[history.size() - 1];   // [OPERATOR OVERLOAD] RingBuffer::operator[]
}

// [Qt] preferred widget size for layout managers
QSize CpuGraphWidget::sizeHint() const {
    return QSize(600, 180);
}

// [OVERRIDE] Qt calls this whenever the widget must be redrawn
// [POLYMORPHISM] virtual override — Qt dispatches this at runtime
void CpuGraphWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    drawBackground(painter);
    drawGrid(painter);
    drawGraph(painter);
    drawLabels(painter);
    drawCurrentValue(painter);
}

// Fills the widget background
void CpuGraphWidget::drawBackground(QPainter& painter) {
    painter.fillRect(rect(), backgroundColor);
}

// Draws horizontal reference lines at 0%, 25%, 50%, 75%, 100%
void CpuGraphWidget::drawGrid(QPainter& painter) {
    painter.setPen(QPen(gridColor, 1, Qt::DotLine));

    int w = width();
    int h = height();
    int padding = 30;

    for (int pct : {25, 50, 75, 100}) {
        int y = padding + static_cast<int>((100 - pct) / 100.0 * (h - 2 * padding));
        painter.drawLine(padding, y, w - 5, y);
    }
}

// Draws the CPU usage line and filled area below it
void CpuGraphWidget::drawGraph(QPainter& painter) {
    int n = history.size();
    if (n < 2) return;

    int w       = width();
    int h       = height();
    int padding = 30;
    int graphW  = w - padding - 5;
    int graphH  = h - 2 * padding;

    // Build a path for the filled area
    QPainterPath fillPath;
    QPainterPath linePath;

    auto xFor = [&](int i) -> double {
        return padding + (static_cast<double>(i) / (history.getCapacity() - 1)) * graphW;
    };
    auto yFor = [&](double val) -> double {
        return padding + (1.0 - val / 100.0) * graphH;
    };

    // Start fill path at the bottom-left corner
    double x0 = xFor(0);
    double y0 = yFor(history[0]);   // [OPERATOR OVERLOAD] RingBuffer::operator[]
    fillPath.moveTo(x0, padding + graphH);
    fillPath.lineTo(x0, y0);
    linePath.moveTo(x0, y0);

    for (int i = 1; i < n; ++i) {
        double x = xFor(i);
        double y = yFor(history[i]);   // [OPERATOR OVERLOAD]
        fillPath.lineTo(x, y);
        linePath.lineTo(x, y);
    }

    // Close fill path back to bottom
    fillPath.lineTo(xFor(n - 1), padding + graphH);
    fillPath.closeSubpath();

    // Draw the gradient fill
    QLinearGradient grad(0, padding, 0, padding + graphH);
    grad.setColorAt(0.0, fillColor);
    grad.setColorAt(1.0, QColor(0, 0, 0, 0));
    painter.fillPath(fillPath, grad);

    // Draw the line on top
    painter.setPen(QPen(graphColor, 2));
    painter.drawPath(linePath);
}

// Draws percentage labels on the left axis
void CpuGraphWidget::drawLabels(QPainter& painter) {
    painter.setPen(QColor(160, 160, 180));
    painter.setFont(QFont("Monospace", 8));

    int h       = height();
    int padding = 30;
    int graphH  = h - 2 * padding;

    for (int pct : {0, 25, 50, 75, 100}) {
        int y = padding + static_cast<int>((100 - pct) / 100.0 * graphH);
        painter.drawText(2, y + 4, QString::number(pct) + "%");
    }
}

// Draws the current CPU value in the top-right corner
void CpuGraphWidget::drawCurrentValue(QPainter& painter) {
    double val = currentValue();
    QString text = QString("CPU: %1%").arg(val, 0, 'f', 1);

    painter.setPen(graphColor);
    painter.setFont(QFont("Monospace", 10, QFont::Bold));
    painter.drawText(rect().adjusted(0, 4, -8, 0),
                     Qt::AlignTop | Qt::AlignRight, text);
}
