// MainWindow.h
 // ============
 // Description: Collect all as Main window and use Qtimer for refresh.

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


// MainWindow
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    // parametrize constrictor
    explicit MainWindow(QWidget* parent = nullptr);

    // Destructor
    ~MainWindow() override;

private slots:
    // slots for QT timer
    void onTimerTick();

    // called when user clicks row in the process table
    void onProcessRowClicked(const QModelIndex& index);

    // called when user changes refresh rate combo box
    void onRefreshRateChanged(int comboIndex);

    // called when user types in filter box
    void onFilterChanged(const QString& text);

    // Called when column header is clicked for sorting
    void onSortIndicatorChanged(int logicalIndex, Qt::SortOrder order);

    // Show About dialog
    void onAboutClicked();

private:

    // compositions of MainWindow

    CpuReader cpuReader;

    ProcessReader processReader;

    ProcessList processList;

    // Qt pointers
    CpuGraphWidget*    cpuGraph;
    ProcessTableModel* tableModel;
    QTableView*        tableView;
    QProgressBar*      cpuBar;
    QLabel*            cpuLabel;
    QLabel*            processCountLabel;
    QComboBox*         refreshRateCombo;
    QLineEdit*         filterEdit;
    QTimer*            refreshTimer;

    // Current refresh interval in milliseconds default: 1000
    int refreshIntervalMs;

    // private setup helpers
    void setupMenuBar();
    void setupCentralWidget();
    void setupStatusBar();
    void setupConnections();
    void applyDarkStyle();

    // private update helpers
    void updateCpuDisplay(double pct);
    void updateProcessTable();
};
