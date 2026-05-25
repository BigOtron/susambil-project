// MainWindow.cpp
//  ==============
//  Description: implementation of the main window. Builds all widgets programmatically,
//               connects signals/slots, and drives the 1-second refresh loop.


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

// parametrize constructor
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
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
      refreshIntervalMs(1000)   // default 1 second
{
    Logger::getInstance()->logInfo("MainWindow constructing");

    setupMenuBar();
    setupCentralWidget();
    setupStatusBar();
    setupConnections();
    applyDarkStyle();

    // start the refresh timer immediately
    refreshTimer->start(refreshIntervalMs);

    Logger::getInstance()->logInfo("MainWindow ready"); // logger message about starting programm
}

// destructor
MainWindow::~MainWindow() {
    if (refreshTimer && refreshTimer->isActive()) {
        refreshTimer->stop();
    }
    Logger::getInstance()->logInfo("MainWindow destroyed"); // logger message about closing the programm
}

// build menu bar with File, View, and Help menus
void MainWindow::setupMenuBar() {
    QMenuBar* mb = menuBar();

    // File menu
    QMenu* fileMenu = mb->addMenu("&File");
    QAction* quitAction = fileMenu->addAction("&Quit");
    connect(quitAction, &QAction::triggered, this, &QMainWindow::close);

    // View menu - refresh rate
    QMenu* viewMenu = mb->addMenu("&View");
    QAction* refreshFast   = viewMenu->addAction("Refresh: 500ms");
    QAction* refreshNormal = viewMenu->addAction("Refresh: 1000ms");
    QAction* refreshSlow   = viewMenu->addAction("Refresh: 2000ms");

    connect(refreshFast,   &QAction::triggered, this, [this]{ onRefreshRateChanged(0); });
    connect(refreshNormal, &QAction::triggered, this, [this]{ onRefreshRateChanged(1); });
    connect(refreshSlow,   &QAction::triggered, this, [this] { onRefreshRateChanged(2); });

    // Help menu
    QMenu* helpMenu = mb->addMenu("&Help");
    QAction* aboutAction = helpMenu->addAction("&About");
    connect(aboutAction, &QAction::triggered, this, &MainWindow::onAboutClicked);
}

// build all central widget and layout
void MainWindow::setupCentralWidget() {
    // Top info bar
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
    // composition of CpuGraphWidget
    cpuGraph = new CpuGraphWidget(this);

    QGroupBox* graphBox = new QGroupBox("CPU Usage History (last 60 s)", this);
    QVBoxLayout* graphLayout = new QVBoxLayout(graphBox);
    graphLayout->setContentsMargins(4, 12, 4, 4);
    graphLayout->addWidget(cpuGraph);

    // Process table
    // composition of ProcessTableModel
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

    // splitter: graph on top, table below
    QSplitter* splitter = new QSplitter(Qt::Vertical, this);
    splitter->addWidget(graphBox);
    splitter->addWidget(tableBox);
    splitter->setStretchFactor(0, 2);
    splitter->setStretchFactor(1, 5);

    // collect central widget
    QWidget* central = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(6, 6, 6, 6);
    mainLayout->setSpacing(4);
    mainLayout->addWidget(topBar);
    mainLayout->addWidget(splitter, 1);

    setCentralWidget(central);

    // timer - Qt manages its lifetime as child of this window
    refreshTimer = new QTimer(this);
}

// Sets up status bar at the bottom
void MainWindow::setupStatusBar() {
    statusBar()->showMessage("Starting…");
}

// Connects all signals to their slots
void MainWindow::setupConnections() {
    connect(refreshTimer, &QTimer::timeout,
            this,         &MainWindow::onTimerTick);

    connect(tableView->selectionModel(),
            &QItemSelectionModel::currentRowChanged,
            this,
            [this](const QModelIndex& current, const QModelIndex&) {
                onProcessRowClicked(current);
            });

    connect(refreshRateCombo,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            &MainWindow::onRefreshRateChanged);

    connect(filterEdit, &QLineEdit::textChanged,
            this,       &MainWindow::onFilterChanged);

    connect(tableView->horizontalHeader(),
            &QHeaderView::sortIndicatorChanged,
            this,
            &MainWindow::onSortIndicatorChanged);
}

// Applies dark theme stylesheet to the whole window
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

// exceptions
void MainWindow::onTimerTick() {
    try {
        // CPU update
        cpuReader.readData();   // call composed object's method
        double cpuPct = cpuReader.getCpuData().getUsagePercent();

        // We collect updatable objects throught their base-class pointer and call update()
        std::vector<SystemInfo*> monitors;

        updateCpuDisplay(cpuPct);

        // process update
        processReader.readAll(processList);
        updateProcessTable();

        // status bar
        statusBar()->showMessage(
            QString("CPU: %1%  |  Processes: %2  |  Log messages: %3")
                .arg(cpuPct, 0, 'f', 1)
                .arg(processList.count())
                .arg(Logger::getMessageCount())   // static member access
        );

    } catch (const MonitorException& e) {
        Logger::getInstance()->logError(e.what());
        statusBar()->showMessage(QString("Error: ") + e.what());
    } catch (const std::exception& e) {
        Logger::getInstance()->logError(std::string("Timer tick: ") + e.what());
    }
}

// updates the CPU progress bar and label
void MainWindow::updateCpuDisplay(double pct) {
    cpuLabel->setText(QString("CPU: %1%").arg(pct, 0, 'f', 1));
    cpuBar->setValue(static_cast<int>(pct));
    cpuGraph->addDataPoint(pct);   // push to graph widget

    // Color the bar red when CPU > 80%
    if (pct > 80.0) {
        cpuBar->setStyleSheet("QProgressBar::chunk { background-color: #cc2020; }");
    } else if (pct > 50.0) {
        cpuBar->setStyleSheet("QProgressBar::chunk { background-color: #cc8800; }");
    } else {
        cpuBar->setStyleSheet("");
    }
}

// pushes the current process into the table model
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

// show tooltip-style status message when table row is clicked
void MainWindow::onProcessRowClicked(const QModelIndex& index) {
    if (!index.isValid()) return;
    int row = index.row();
    if (row < 0 || row >= tableModel->rowCount()) return;

    ProcessInfo proc = tableModel->getProcess(row);
    // use stream output for the status bar info
    std::ostringstream oss;
    oss << proc;   // calls operator<<(ostream&, ProcessInfo&)
    statusBar()->showMessage(QString::fromStdString(oss.str()), 4000);
}

// changes the QTimer interval based on the combo box selection
void MainWindow::onRefreshRateChanged(int comboIndex) {
    static const int rates[] = {500, 1000, 2000};
    if (comboIndex < 0 || comboIndex > 2) return;

    refreshIntervalMs = rates[comboIndex];
    refreshTimer->setInterval(refreshIntervalMs);
    Logger::getInstance()->logInfo(
        "Refresh rate changed to " + std::to_string(refreshIntervalMs) + " ms");
}

// re-applies the process table filter whenever the user types
void MainWindow::onFilterChanged(const QString& /*text*/) {
    updateProcessTable();
}

// sorts the process list when a column header is clicked
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

// shows the About modal dialog
void MainWindow::onAboutClicked() {
    AboutDialog dlg(this);   // MainWindow creates an AboutDialog
    dlg.exec();              // blocks until dialog is closed
}
