/*
 * ProcessTableModel.h
 * ===================
 * Description: Qt Model/View table model that exposes a vector<ProcessInfo> to
 *              QTableView. Implements the required QAbstractTableModel interface.
 *
 * OOP Concepts Demonstrated:
 *   - Inheritance (derives from QAbstractTableModel)
 *   - Composition (has-a vector<ProcessInfo>)
 *   - Override of Qt pure virtual functions
 *   - Encapsulation
 *
 * Lecture Reference: Lecture 12-14 (Qt GUI, Model/View)
 *
 * Author: OOP 2 Project Team
 * Course: OOP 2 (MSC1052) — Spring 2026
 */

#pragma once

#include <QAbstractTableModel>
#include <QColor>
#include <vector>
#include "../core/ProcessInfo.h"

/**
 * @class ProcessTableModel
 * @brief Bridges ProcessInfo data to Qt's Model/View table display.
 *
 * Demonstrates: Qt Model/View architecture, inheritance, virtual overrides.
 * Corresponds to: Lectures 12-14
 */
// [INHERITANCE] ProcessTableModel IS-A QAbstractTableModel
class ProcessTableModel : public QAbstractTableModel {
    Q_OBJECT   // [Qt] required macro for signals/slots

private:
    // [COMPOSITION] model HAS-A local copy of the process vector
    std::vector<ProcessInfo> processes;

    // Column index constants for readability
    static const int COL_PID     = 0;
    static const int COL_NAME    = 1;
    static const int COL_CPU     = 2;
    static const int COL_MEM     = 3;
    static const int COL_THREADS = 4;
    static const int COL_STATE   = 5;
    static const int COL_COUNT   = 6;

public:
    // [PARAMETERIZED CONSTRUCTOR] parent passed to QAbstractTableModel
    explicit ProcessTableModel(QObject* parent = nullptr);

    // [DESTRUCTOR]
    ~ProcessTableModel() override;

    // ---- QAbstractTableModel required overrides ----

    // [OVERRIDE] returns number of rows (= number of processes)
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    // [OVERRIDE] returns number of columns (6: PID, Name, CPU, Mem, Threads, State)
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    // [OVERRIDE] returns cell data for the given role
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    // [OVERRIDE] returns column header labels
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // ---- Custom public methods ----

    /**
     * @brief Replaces the internal process list and triggers a full table refresh.
     */
    void setProcessList(const std::vector<ProcessInfo>& list);

    /**
     * @brief Returns the ProcessInfo at the given row index.
     */
    ProcessInfo getProcess(int row) const;
};
