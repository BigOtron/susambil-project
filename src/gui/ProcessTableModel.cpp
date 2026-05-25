
// ProcessTableModel.cpp
// =====================
// Description: Implementation ProcessTableModel class - get cell data, headers,
// and color-coding for QTableView throught Qt Model/View architecture.


#include "ProcessTableModel.h" // use Process Table Model header
#include "../utils/Formatter.h" // use Formatter header
#include <QString>
#include <QBrush>
#include <QFont>

// parametrize contructor
ProcessTableModel::ProcessTableModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

// destructor
ProcessTableModel::~ProcessTableModel() {}

// count of rows == current processes
int ProcessTableModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;   // check is it valid or not?
    return static_cast<int>(processes.size()); // size_t type to int.
}

// we have only 6 colums like PID, name and other 4
int ProcessTableModel::columnCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0; // check is it valid or not?
    return COL_COUNT; // return always 6 because count of this is 6
}

// return data regarding role type
QVariant ProcessTableModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) return QVariant(); // check is it valid or not?

    // implement row and col
    int row = index.row();
    int col = index.column();

    if (row < 0 || row >= static_cast<int>(processes.size())) return QVariant();

    const ProcessInfo& proc = processes[static_cast<std::size_t>(row)];

    // this is for displaying
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

    // use TextAlignmentRole for correctozition position of texts and numbers.
    if (role == Qt::TextAlignmentRole) {
        if (col == COL_PID || col == COL_CPU || col == COL_MEM || col == COL_THREADS) {
            return static_cast<int>(Qt::AlignRight | Qt::AlignVCenter);
        }
        return static_cast<int>(Qt::AlignLeft | Qt::AlignVCenter);
    }

    // color for zombie and stopped processes
    if (role == Qt::ForegroundRole) {
        char state = proc.getState();
        if (state == 'Z') return QBrush(QColor(200, 0, 0));    // zombie — red
        if (state == 'T') return QBrush(QColor(150, 150, 0));  // stopped — yellow
        if (proc.getCpuUsage() > 50.0)
            return QBrush(QColor(220, 80, 0));                  // high CPU — orange
    }

    return QVariant();
}

// return the column header text
QVariant ProcessTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) return QVariant(); // check is it this role is DisplayRole?

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

// it is for redraw.
void ProcessTableModel::setProcessList(const std::vector<ProcessInfo>& list) {
    beginResetModel(); // it is like QT close your eye
    processes = list;  // ok we are changed
    endResetModel();   // QT open your eye
}

// returns of ProcessInfo type.
ProcessInfo ProcessTableModel::getProcess(int row) const {
    return processes[static_cast<std::size_t>(row)];
}
