/*
 * AboutDialog.cpp
 * ===============
 * Description: Implementation of the About dialog. Programmatically creates
 *              the layout with labels and a close button.
 *
 * OOP Concepts Demonstrated:
 *   - Qt dialog (QDialog inheritance)
 *   - Signals and slots (connect button to accept)
 *   - Layout managers (QVBoxLayout)
 *
 * Lecture Reference: Lectures 12-14 (Qt GUI)
 *
 * Author: OOP 2 Project Team
 * Course: OOP 2 (MSC1052) — Spring 2026
 */

#include "AboutDialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFont>

// [PARAMETERIZED CONSTRUCTOR] builds the dialog layout programmatically
// [CONSTRUCTOR CHAINING] QDialog base initialized with parent
AboutDialog::AboutDialog(QWidget* parent)
    : QDialog(parent)   // [INHERITANCE] constructor chaining to QDialog
{
    setWindowTitle("About Linux System Activity Monitor");
    setFixedSize(420, 280);
    setModal(true);

    // [Qt] layout manager — arranges widgets vertically
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(10);
    layout->setContentsMargins(20, 20, 20, 20);

    // Title label
    QLabel* title = new QLabel("Linux System Activity Monitor", this);
    QFont titleFont = title->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    title->setFont(titleFont);
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    // Version
    QLabel* version = new QLabel("Version 1.0", this);
    version->setAlignment(Qt::AlignCenter);
    layout->addWidget(version);

    layout->addSpacing(10);

    // Course info
    QLabel* course = new QLabel(
        "Course: OOP 2 (MSC1052) — Spring 2026\n"
        "Author: OOP 2 Project Team\n\n"
        "Reads real-time system data from the Linux\n"
        "/proc virtual filesystem and displays it\n"
        "in a Qt6 graphical interface.",
        this);
    course->setAlignment(Qt::AlignCenter);
    layout->addWidget(course);

    layout->addSpacing(10);

    // Close button
    QPushButton* closeBtn = new QPushButton("Close", this);
    layout->addWidget(closeBtn, 0, Qt::AlignCenter);

    // [Qt SIGNALS & SLOTS] connect button click to dialog close
    connect(closeBtn, &QPushButton::clicked, this, &AboutDialog::accept);
}

// [DESTRUCTOR]
AboutDialog::~AboutDialog() {}
