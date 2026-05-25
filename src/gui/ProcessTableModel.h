// ProcessTableModel.h
// ===================
// Description: This class holds list of processes and shows them
//              in a table on the screen. It works with Qt's table system.
// in this header file we use QAbstractTableModel - it is for create QT tables

#pragma once

#include <QAbstractTableModel>
#include <QColor>
#include <vector>
#include "../core/ProcessInfo.h" // using Process Info header file


class ProcessTableModel : public QAbstractTableModel {
    Q_OBJECT

private:
    // composition of processes from FrecessInfo class
    std::vector<ProcessInfo> processes;

    // indexing all process name. for one thing for name index number is 1
    // all count is 6
    static const int COL_PID     = 0;
    static const int COL_NAME    = 1;
    static const int COL_CPU     = 2;
    static const int COL_MEM     = 3;
    static const int COL_THREADS = 4;
    static const int COL_STATE   = 5;
    static const int COL_COUNT   = 6;

public:
    // param. contructor parent to QAbs table model
    explicit ProcessTableModel(QObject* parent = nullptr);

    // Desctuctor.
    ~ProcessTableModel() override;

    // QModelIndex - it is like excel. For example B3 or F6 and QModelIndex work like that and we use row and colimns
    //
    // return count of rows and it is equal to count of processes.
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    // return count of columns and it is for example PID, Memory and ...
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    // return cell data for given role.
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    // returns column header label
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // setter func to set all process by list
    void setProcessList(const std::vector<ProcessInfo>& list);

    // getter func and return ProcessInfo type
    ProcessInfo getProcess(int row) const;
};