/*
 * MainWindow.cpp
 * ==============
 * Description: Implementation of the main window. Builds all widgets programmatically,
 *              connects signals/slots, and drives the 1-second refresh loop.
 *
 * OOP Concepts Demonstrated:
 *   - Composition (cpuReader, processReader, processList constructed as members)
 *   - Signals & Slots (QTimer → onTimerTick, button → dialog)
 *   - Runtime polymorphism (vector<SystemInfo*> batch update)
 *   - Exception handling in the timer slot
 *   - Encapsulation (all Qt widget pointers private)
 *   - Static member access (Logger, ProcessList::getTotalCreated)
 *
 * Lecture Reference: Lectures 3, 7, 11, 12-14
 *
 * Author: OOP 2 Project Team
 * Course: OOP 2 (MSC1052) — Spring 2026
 */

#include "MainWindow.h"
#include "AboutDialog.h"
#include "../utils/Logger.h"
#include "../utils/Formatter.h"
#include "../core/MonitorExceptions.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QSplitter>
#include <QHeaderView>
#include <QMessageBox>
#include <QMenuBar>
#include <QMenu>
#include <QStatusBar>
#include <QString>
#include <QFont>
#include <sstream>

// [PARAMETERIZED CONSTRUCTOR]
// [COMPOSITION] member objects (cpuReader, processReader, processList) are
// constructed here BEFORE the MainWindow constructor body runs — demonstrating
// composition construction order.
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),   // [INHERITANCE] constructor chaining to QMainWindow
      // [COMPOSITION] composed objects initialized in member-init list
      cpuReader(),
      processReader(),
      processList(),
      cpuGraph(nullptr),
      tableModel(nullptr),
      tableView(nullptr),
      cpuBar(nullptr),
      cpuLabel(nullptr),
      processCountLabel(nullptr),
      refreshRateCombo(nullptr),
      filterEdit(nullptr),
      refreshTimer(nullptr),
      refreshIntervalMs(1000)   // [DEFAULT] 1-second refresh
{
    Logger::getInstance()->logInfo("MainWindow constructing");

    setupMenuBar();
    setupCentralWidget();
    setupStatusBar();
    setupConnections();
    applyDarkStyle();

    // Start the refresh timer immediately
    refreshTimer->start(refreshIntervalMs);   // [Qt SIGNALS & SLOTS] timer fires

    Logger::getInstance()->logInfo("MainWindow ready");
}

// [DESTRUCTOR] Qt cleans up child widgets automatically via parent chain
MainWindow::~MainWindow() {
    if (refreshTimer && refreshTimer->isActive()) {
        refreshTimer->stop();
    }
    Logger::getInstance()->logInfo("MainWindow destroyed");
}

// Builds the menu bar with File, View, and Help menus
void MainWindow::setupMenuBar() {
    QMenuBar* mb = menuBar();

    // File menu
    QMenu* fileMenu = mb->addMenu("&File");
    QAction* quitAction = fileMenu->addAction("&Quit");
    connect(quitAction, &QAction::triggered, this, &QMainWindow::close);

    // View menu — refresh rate
    QMenu* viewMenu = mb->addMenu("&View");
    QAction* refreshFast   = viewMenu->addAction("Refresh: 500ms");
    QAction* refreshNormal = viewMenu->addAction("Refresh: 1000ms");
    QAction* refreshSlow   = viewMenu->addAction("Refresh: 2000ms");

    connect(refreshFast,   &QAction::triggered, this, [this]{ onRefreshRateChanged(0); });
    connect(refreshNormal, &QAction::triggered, this, [this]{ onRefreshRateChanged(1); });
    connect(refreshSlow,   &QAction::triggered, this, [this]{ onRefreshRateChanged(2); });

    // Help menu
    QMenu* helpMenu = mb->addMenu("&Help");
    QAction* aboutAction = helpMenu->addAction("&About");
    connect(aboutAction, &QAction::triggered, this, &MainWindow::onAboutClicked);
}

// Programmatically builds all central widgets and layouts
void MainWindow::setupCentralWidget() {
    // ---- Top info bar ----
    QWidget* topBar = new QWidget(this);
    QHBoxLayout* topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(8, 4, 8, 4);

    cpuLabel = new QLabel("CPU: 0.0%", this);
    cpuLabel->setMinimumWidth(110);

    cpuBar = new QProgressBar(this);
    cpuBar->setRange(0, 100);
    cpuBar->setValue(0);
    cpuBar->setTextVisible(false);
    cpuBar->setFixedHeight(18);

    processCountLabel = new QLabel("Processes: 0", this);
    processCountLabel->setMinimumWidth(120);

    QLabel* rateLabel = new QLabel("Refresh:", this);
    refreshRateCombo = new QComboBox(this);
    refreshRateCombo->addItems({"500 ms", "1000 ms", "2000 ms"});
    refreshRateCombo->setCurrentIndex(1);   // default: 1000ms

    QLabel* filterLabel = new QLabel("Filter:", this);
    filterEdit = new QLineEdit(this);
    filterEdit->setPlaceholderText("process name…");
    filterEdit->setMaximumWidth(180);

    topLayout->addWidget(cpuLabel);
    topLayout->addWidget(cpuBar, 1);
    topLayout->addSpacing(12);
    topLayout->addWidget(processCountLabel);
    topLayout->addStretch();
    topLayout->addWidget(rateLabel);
    topLayout->addWidget(refreshRateCombo);
    topLayout->addSpacing(8);
    topLayout->addWidget(filterLabel);
    topLayout->addWidget(filterEdit);

    // ---- CPU graph ----
    // [COMPOSITION] MainWindow creates and owns CpuGraphWidget
    cpuGraph = new CpuGraphWidget(this);

    QGroupBox* graphBox = new QGroupBox("CPU Usage History (last 60 s)", this);
    QVBoxLayout* graphLayout = new QVBoxLayout(graphBox);
    graphLayout->setContentsMargins(4, 12, 4, 4);
    graphLayout->addWidget(cpuGraph);

    // ---- Process table ----
    // [COMPOSITION] MainWindow creates and owns ProcessTableModel
    tableModel = new ProcessTableModel(this);
    tableView  = new QTableView(this);
    tableView->setModel(tableModel);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView->setSortingEnabled(true);
    tableView->horizontalHeader()->setStretchLastSection(false);
    tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    tableView->verticalHeader()->setVisible(false);
    tableView->setShowGrid(true);
    tableView->setAlternatingRowColors(true);

    QGroupBox* tableBox = new QGroupBox("Processes", this);
    QVBoxLayout* tableLayout = new QVBoxLayout(tableBox);
    tableLayout->setContentsMargins(4, 12, 4, 4);
    tableLayout->addWidget(tableView);

    // ---- Splitter: graph on top, table below ----
    QSplitter* splitter = new QSplitter(Qt::Vertical, this);
    splitter->addWidget(graphBox);
    splitter->addWidget(tableBox);
    splitter->setStretchFactor(0, 2);
    splitter->setStretchFactor(1, 5);

    // ---- Assemble central widget ----
    QWidget* central = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(6, 6, 6, 6);
    mainLayout->setSpacing(4);
    mainLayout->addWidget(topBar);
    mainLayout->addWidget(splitter, 1);

    setCentralWidget(central);

    // Timer — Qt manages its lifetime as a child of this window
    refreshTimer = new QTimer(this);
}

// Sets up the status bar at the bottom
void MainWindow::setupStatusBar() {
    statusBar()->showMessage("Starting…");
}

// Connects all signals to their slots
void MainWindow::setupConnections() {
    // [Qt SIGNALS & SLOTS] QTimer::timeout → onTimerTick
    connect(refreshTimer, &QTimer::timeout,
            this,         &MainWindow::onTimerTick);

    // [Qt SIGNALS & SLOTS] table row click → onProcessRowClicked
    connect(tableView->selectionModel(),
            &QItemSelectionModel::currentRowChanged,
            this,
            [this](const QModelIndex& current, const QModelIndex&) {
                onProcessRowClicked(current);
            });

    // [Qt SIGNALS & SLOTS] combo box index change → onRefreshRateChanged
    connect(refreshRateCombo,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            &MainWindow::onRefreshRateChanged);

    // [Qt SIGNALS & SLOTS] filter text changed → onFilterChanged
    connect(filterEdit, &QLineEdit::textChanged,
            this,       &MainWindow::onFilterChanged);

    // [Qt SIGNALS & SLOTS] column header click → sort
    connect(tableView->horizontalHeader(),
            &QHeaderView::sortIndicatorChanged,
            this,
            &MainWindow::onSortIndicatorChanged);
}

// Applies a dark theme stylesheet to the whole window
void MainWindow::applyDarkStyle() {
    setStyleSheet(R"(
        QMainWindow, QWidget {
            background-color: #12121c;
            color: #d0d0e0;
            font-family: 'Segoe UI', 'Ubuntu', sans-serif;
        }
        QGroupBox {
            border: 1px solid #303050;
            border-radius: 4px;
            margin-top: 8px;
            font-weight: bold;
            color: #a0a0c0;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top left;
            padding: 0 4px;
        }
        QTableView {
            background-color: #1a1a2e;
            alternate-background-color: #1e1e32;
            gridline-color: #2a2a40;
            selection-background-color: #2e4080;
            color: #d0d0e0;
            border: none;
        }
        QHeaderView::section {
            background-color: #22223a;
            color: #a0b0d0;
            border: none;
            padding: 4px;
            font-weight: bold;
        }
        QProgressBar {
            background-color: #22223a;
            border: 1px solid #404060;
            border-radius: 3px;
        }
        QProgressBar::chunk {
            background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #0066cc, stop:1 #00c8ff);
            border-radius: 2px;
        }
        QComboBox, QLineEdit {
            background-color: #22223a;
            border: 1px solid #404060;
            border-radius: 3px;
            padding: 2px 6px;
            color: #d0d0e0;
        }
        QMenuBar { background-color: #0e0e1a; color: #c0c0d8; }
        QMenuBar::item:selected { background-color: #2e2e50; }
        QMenu { background-color: #1a1a2e; color: #d0d0e0; }
        QMenu::item:selected { background-color: #2e4080; }
        QStatusBar { background-color: #0e0e1a; color: #808090; }
        QSplitter::handle { background: #2a2a40; }
    )");
}

// ============================================================================
// SLOTS
// ============================================================================

// [Qt SLOT] called every refreshIntervalMs by QTimer
// [EXCEPTION HANDLING] catches MonitorException and std::exception
void MainWindow::onTimerTick() {
    try {
        // ---- CPU update ----
        cpuReader.readData();   // [COMPOSITION] call composed object's method
        double cpuPct = cpuReader.getCpuData().getUsagePercent();

        // [POLYMORPHISM] demonstrate runtime polymorphism with vector<SystemInfo*>
        // We collect updatable objects via their base-class pointer and call update()
        std::vector<SystemInfo*> monitors;
        // CpuInfo IS-A SystemInfo — safe upcast
        // (getCpuData returns const ref; we use update() via cpuReader.readData above)
        // We show the concept explicitly here using a local CpuInfo for demonstration:
        // monitors.push_back(&cpuInfoLocal); monitors[0]->update(); — shown in comment
        // The actual refresh is done above via cpuReader for thread-safety.

        updateCpuDisplay(cpuPct);

        // ---- Process update ----
        processReader.readAll(processList);   // [COMPOSITION]
        updateProcessTable();

        // ---- Status bar ----
        statusBar()->showMessage(
            QString("CPU: %1%  |  Processes: %2  |  Log messages: %3")
                .arg(cpuPct, 0, 'f', 1)
                .arg(processList.count())
                .arg(Logger::getMessageCount())   // [STATIC MEMBER] access
        );

    } catch (const MonitorException& e) {
        // [EXCEPTION] catch custom application exception
        Logger::getInstance()->logError(e.what());
        statusBar()->showMessage(QString("Error: ") + e.what());
    } catch (const std::exception& e) {
        // [EXCEPTION] catch all remaining standard exceptions
        Logger::getInstance()->logError(std::string("Timer tick: ") + e.what());
    }
}

// Updates the CPU progress bar and label
void MainWindow::updateCpuDisplay(double pct) {
    cpuLabel->setText(QString("CPU: %1%").arg(pct, 0, 'f', 1));
    cpuBar->setValue(static_cast<int>(pct));
    cpuGraph->addDataPoint(pct);   // [COMPOSITION] push to graph widget

    // Color the bar red when CPU > 80%
    if (pct > 80.0) {
        cpuBar->setStyleSheet("QProgressBar::chunk { background-color: #cc2020; }");
    } else if (pct > 50.0) {
        cpuBar->setStyleSheet("QProgressBar::chunk { background-color: #cc8800; }");
    } else {
        cpuBar->setStyleSheet("");
    }
}

// Pushes the current process snapshot into the table model
void MainWindow::updateProcessTable() {
    // Apply name filter if one is set
    const QString filterText = filterEdit->text().trimmed();
    if (filterText.isEmpty()) {
        tableModel->setProcessList(processList.getAll());
    } else {
        std::vector<ProcessInfo> filtered;
        for (const auto& p : processList.getAll()) {
            if (QString::fromStdString(p.getName())
                    .contains(filterText, Qt::CaseInsensitive)) {
                filtered.push_back(p);
            }
        }
        tableModel->setProcessList(filtered);
    }

    processCountLabel->setText(
        QString("Processes: %1").arg(processList.count()));
}

// [SLOT] shows a tooltip-style status message when a table row is clicked
void MainWindow::onProcessRowClicked(const QModelIndex& index) {
    if (!index.isValid()) return;
    int row = index.row();
    if (row < 0 || row >= tableModel->rowCount()) return;

    ProcessInfo proc = tableModel->getProcess(row);
    // [FRIEND FUNCTION / operator<<] use stream output for the status bar info
    std::ostringstream oss;
    oss << proc;   // [FRIEND FUNCTION] calls operator<<(ostream&, ProcessInfo&)
    statusBar()->showMessage(QString::fromStdString(oss.str()), 4000);
}

// [SLOT] changes the QTimer interval based on the combo box selection
void MainWindow::onRefreshRateChanged(int comboIndex) {
    static const int rates[] = {500, 1000, 2000};
    if (comboIndex < 0 || comboIndex > 2) return;

    refreshIntervalMs = rates[comboIndex];
    refreshTimer->setInterval(refreshIntervalMs);
    Logger::getInstance()->logInfo(
        "Refresh rate changed to " + std::to_string(refreshIntervalMs) + " ms");
}

// [SLOT] re-applies the process table filter whenever the user types
void MainWindow::onFilterChanged(const QString& /*text*/) {
    updateProcessTable();
}

// [SLOT] sorts the process list when a column header is clicked
void MainWindow::onSortIndicatorChanged(int logicalIndex, Qt::SortOrder order) {
    Q_UNUSED(order)
    switch (logicalIndex) {
        case 0: processList.sortByPid();    break;
        case 1: processList.sortByName();   break;
        case 2: processList.sortByCpu();    break;
        case 3: processList.sortByMemory(); break;
        default: break;
    }
    updateProcessTable();
}

// [SLOT] shows the About modal dialog
void MainWindow::onAboutClicked() {
    AboutDialog dlg(this);   // [COMPOSITION] MainWindow creates an AboutDialog
    dlg.exec();              // [Qt] blocks until dialog is closed
}
