/*
 * MainWindow.h
 * ============
 * Description: Main application window. Ties all core and GUI components together.
 *              Uses a QTimer to refresh data every 1000ms via signal/slot.
 *
 * OOP Concepts Demonstrated:
 *   - Inheritance (QMainWindow)
 *   - Composition (has-a CpuReader, ProcessReader, ProcessList, etc.)
 *   - Signals & Slots (Qt mechanism)
 *   - Encapsulation
 *   - Runtime polymorphism (vector<SystemInfo*> for batch update)
 *   - Exception handling in slots
 *
 * Lecture Reference: Lectures 3 (Composition), 7 (Polymorphism), 12-14 (Qt GUI)
 *
 * Author: OOP 2 Project Team
 * Course: OOP 2 (MSC1052) — Spring 2026
 */

#pragma once

#include <QMainWindow>
#include <QTimer>
#include <QLabel>
#include <QProgressBar>
#include <QTableView>
#include <QComboBox>
#include <QLineEdit>
#include <QSplitter>
#include <QStatusBar>
#include <QMenuBar>
#include <QAction>
#include <vector>

#include "../core/CpuReader.h"
#include "../core/ProcessReader.h"
#include "../core/ProcessList.h"
#include "../core/SystemInfo.h"
#include "CpuGraphWidget.h"
#include "ProcessTableModel.h"

/**
 * @class MainWindow
 * @brief Main application window — composes all subsystems and drives the UI.
 *
 * Demonstrates: composition, Qt inheritance, signals/slots, polymorphism.
 * Corresponds to: Lectures 3, 7, 12-14
 */
// [INHERITANCE] MainWindow IS-A QMainWindow
class MainWindow : public QMainWindow {
    Q_OBJECT   // [Qt] required for signals/slots

public:
    // [PARAMETERIZED CONSTRUCTOR]
    explicit MainWindow(QWidget* parent = nullptr);

    // [DESTRUCTOR]
    ~MainWindow() override;

private slots:
    // [Qt SIGNALS & SLOTS] called by QTimer every refreshIntervalMs milliseconds
    void onTimerTick();

    // Called when user clicks a row in the process table
    void onProcessRowClicked(const QModelIndex& index);

    // Called when the user changes the refresh rate combo box
    void onRefreshRateChanged(int comboIndex);

    // Called when the user types in the filter box
    void onFilterChanged(const QString& text);

    // Called when a column header is clicked for sorting
    void onSortIndicatorChanged(int logicalIndex, Qt::SortOrder order);

    // Shows the About dialog
    void onAboutClicked();

private:
    // ---- Composed subsystem objects (has-a relationships) ----

    // [COMPOSITION] MainWindow HAS-A CpuReader
    CpuReader cpuReader;

    // [COMPOSITION] MainWindow HAS-A ProcessReader
    ProcessReader processReader;

    // [COMPOSITION] MainWindow HAS-A ProcessList
    ProcessList processList;

    // [POLYMORPHISM] base-class pointers for batch update — runtime dispatch
    // vector<SystemInfo*> monitors;  // demonstrated in onTimerTick

    // ---- Qt-owned widget pointers (Qt manages lifetime via parent chain) ----
    CpuGraphWidget*    cpuGraph;
    ProcessTableModel* tableModel;
    QTableView*        tableView;
    QProgressBar*      cpuBar;
    QLabel*            cpuLabel;
    QLabel*            processCountLabel;
    QComboBox*         refreshRateCombo;
    QLineEdit*         filterEdit;
    QTimer*            refreshTimer;

    // Current refresh interval in milliseconds (default: 1000)
    int refreshIntervalMs;

    // ---- Private setup helpers ----
    void setupMenuBar();
    void setupCentralWidget();
    void setupStatusBar();
    void setupConnections();
    void applyDarkStyle();

    // ---- Private update helpers ----
    void updateCpuDisplay(double pct);
    void updateProcessTable();
};
