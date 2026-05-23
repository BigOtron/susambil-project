/*
 * main.cpp
 * ========
 * Description: Entry point for Linux System Activity Monitor.
 *              Creates QApplication, initializes the Logger singleton,
 *              instantiates MainWindow, and starts the Qt event loop.
 *
 * OOP Concepts Demonstrated:
 *   - Static member access (Logger::getInstance)
 *   - Exception handling at the top level (catch fatal errors)
 *   - Object lifetime (MainWindow on stack — destructor called on exit)
 *
 * Lecture Reference: Lecture 12 (Qt Application Entry Point)
 *
 * Author: OOP 2 Project Team
 * Course: OOP 2 (MSC1052) — Spring 2026
 */

#include <QApplication>
#include "src/gui/MainWindow.h"
#include "src/utils/Logger.h"
#include "src/core/MonitorExceptions.h"

int main(int argc, char* argv[]) {
    // [Qt] QApplication manages the GUI event loop
    QApplication app(argc, argv);
    app.setApplicationName("Linux System Activity Monitor");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("OOP2 MSC1052");

    // [STATIC MEMBER] Logger::getInstance() — initialize the singleton before anything else
    Logger::getInstance()->logInfo("=== Application starting ===");

    try {
        // [OBJECT] MainWindow created on the stack — destructor called automatically
        MainWindow window;
        window.setWindowTitle("Linux System Activity Monitor");
        window.resize(1280, 720);
        window.show();

        Logger::getInstance()->logInfo("Event loop starting");

        // [Qt] enter the event loop — blocks until the window is closed
        int result = app.exec();

        Logger::getInstance()->logInfo("Event loop exited with code "
                                       + std::to_string(result));
        return result;

    } catch (const MonitorException& e) {
        // [EXCEPTION] fatal application error — log and exit
        Logger::getInstance()->logError(std::string("FATAL MonitorException: ") + e.what());
        return 1;
    } catch (const std::exception& e) {
        // [EXCEPTION] catch any remaining standard exceptions at the top level
        Logger::getInstance()->logError(std::string("FATAL std::exception: ") + e.what());
        return 1;
    }
}
