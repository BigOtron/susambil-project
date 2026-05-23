/*
 * AboutDialog.h
 * =============
 * Description: Simple modal dialog that shows application information.
 *              Demonstrates Qt dialog usage and inheritance from QDialog.
 *
 * OOP Concepts Demonstrated:
 *   - Inheritance (derives from QDialog)
 *   - Encapsulation
 *   - Constructor/Destructor
 *
 * Lecture Reference: Lecture 12-14 (Qt GUI)
 *
 * Author: OOP 2 Project Team
 * Course: OOP 2 (MSC1052) — Spring 2026
 */

#pragma once

#include <QDialog>

/**
 * @class AboutDialog
 * @brief Modal dialog showing app name, version, and course information.
 *
 * Demonstrates: QDialog inheritance, simple Qt widget layout.
 * Corresponds to: Lectures 12-14
 */
// [INHERITANCE] AboutDialog IS-A QDialog
class AboutDialog : public QDialog {
    Q_OBJECT

public:
    // [PARAMETERIZED CONSTRUCTOR]
    explicit AboutDialog(QWidget* parent = nullptr);

    // [DESTRUCTOR]
    ~AboutDialog() override;
};
