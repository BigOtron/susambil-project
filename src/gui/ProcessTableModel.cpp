/*
 * ProcessTableModel.cpp
 * =====================
 * Description: Implementation of ProcessTableModel — supplies cell data, headers,
 *              and color-coding to QTableView via the Qt Model/View architecture.
 *
 * OOP Concepts Demonstrated:
 *   - Qt Model/View (QAbstractTableModel)
 *   - Inheritance and virtual overrides
 *   - Encapsulation
 *   - Composition (has-a vector<ProcessInfo>)
 *
 * Lecture Reference: Lectures 12-14 (Qt GUI)
 *
 * Author: OOP 2 Project Team
 * Course: OOP 2 (MSC1052) — Spring 2026
 */

#include "ProcessTableModel.h"
#include "../utils/Formatter.h"
#include <QString>
#include <QBrush>
#include <QFont>

// [PARAMETERIZED CONSTRUCTOR] passes parent to Qt base class
ProcessTableModel::ProcessTableModel(QObject* parent)
    : QAbstractTableModel(parent)   // [INHERITANCE] constructor chaining to Qt base
{
}

// [DESTRUCTOR]
ProcessTableModel::~ProcessTableModel() {}

// [OVERRIDE] number of rows = number of processes in the current snapshot
int ProcessTableModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;   // Qt convention: root items only
    return static_cast<int>(processes.size());
}

// [OVERRIDE] always 6 columns: PID, Name, CPU%, Memory, Threads, State
int ProcessTableModel::columnCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return COL_COUNT;
}

// [OVERRIDE] returns data for each cell based on the role requested by the view
QVariant ProcessTableModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) return QVariant();

    int row = index.row();
    int col = index.column();

    if (row < 0 || row >= static_cast<int>(processes.size())) return QVariant();

    const ProcessInfo& proc = processes[static_cast<std::size_t>(row)];

    // [Qt] DisplayRole — the text shown in the cell
    if (role == Qt::DisplayRole) {
        switch (col) {
            case COL_PID:     return proc.getPid();
            case COL_NAME:    return QString::fromStdString(proc.getName());
            case COL_CPU:     return QString::fromStdString(
                                  formatDouble(proc.getCpuUsage()) + "%");
            case COL_MEM:     return QString::fromStdString(
                                  formatMemory(proc.getMemoryKb()));
            case COL_THREADS: return proc.getThreadCount();
            case COL_STATE:   return QString(QChar(proc.getState()));
            default:          return QVariant();
        }
    }

    // [Qt] TextAlignmentRole — right-align numeric columns
    // Qt6 requires explicit int cast when returning alignment flags as QVariant
    if (role == Qt::TextAlignmentRole) {
        if (col == COL_PID || col == COL_CPU || col == COL_MEM || col == COL_THREADS) {
            return static_cast<int>(Qt::AlignRight | Qt::AlignVCenter);
        }
        return static_cast<int>(Qt::AlignLeft | Qt::AlignVCenter);
    }

    // [Qt] ForegroundRole — color code zombie and stopped processes
    if (role == Qt::ForegroundRole) {
        char state = proc.getState();
        if (state == 'Z') return QBrush(QColor(200, 0, 0));    // zombie — red
        if (state == 'T') return QBrush(QColor(150, 150, 0));  // stopped — yellow
        if (proc.getCpuUsage() > 50.0)
            return QBrush(QColor(220, 80, 0));                  // high CPU — orange
    }

    return QVariant();
}

// [OVERRIDE] returns the column header text
QVariant ProcessTableModel::headerData(int section, Qt::Orientation orientation,
                                       int role) const
{
    if (role != Qt::DisplayRole) return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
            case COL_PID:     return "PID";
            case COL_NAME:    return "Process Name";
            case COL_CPU:     return "CPU %";
            case COL_MEM:     return "Memory";
            case COL_THREADS: return "Threads";
            case COL_STATE:   return "State";
            default:          return QVariant();
        }
    }
    return QVariant();
}

// Replaces the entire process list and notifies the view to redraw
void ProcessTableModel::setProcessList(const std::vector<ProcessInfo>& list) {
    // [Qt] beginResetModel/endResetModel bracket any structural change
    beginResetModel();
    processes = list;   // copy the new snapshot
    endResetModel();
}

// Returns a copy of the ProcessInfo at the given row
ProcessInfo ProcessTableModel::getProcess(int row) const {
    return processes[static_cast<std::size_t>(row)];
}
