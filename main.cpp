/*
 * main.cpp
 * ========
 * Entry point for the Linux System Activity Monitor.
 * Sets up the Qt app, starts the logger, launches the main window,
 * and runs the event loop until the user closes the program.
 *
 * OOP Concepts:
 *   - Static member access (Logger::getInstance)
 *   - Exception handling for fatal errors at the top level
 *   - Stack object lifetime (MainWindow destructor runs on exit)
 *
 * Author: OOP 2 Project Team
 * Course: OOP 2  — Spring 2026
 */

#include <QApplication>
#include "src/gui/MainWindow.h"
#include "src/utils/Logger.h"
#include "src/core/MonitorExceptions.h"

using namespace std;

int main(int argc, char* argv[]) {
    // Qt requires a QApplication object before any GUI work can happen
    QApplication app(argc, argv);
    app.setApplicationName("Linux System Activity Monitor");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("OOP2 MSC1052");

    // Start the logger first so every event from here on gets recorded
    Logger::getInstance()->logInfo("=== Application starting ===");

    try {
        // MainWindow lives on the stack — Qt and the destructor clean it up on exit
        MainWindow window;
        window.setWindowTitle("Linux System Activity Monitor");
        window.resize(1280, 720);
        window.show();

        Logger::getInstance()->logInfo("Event loop starting");

        // Hands control to Qt — stays here until the window is closed
        int result = app.exec();

        Logger::getInstance()->logInfo("Event loop exited with code "
                                       + to_string(result));
        return result;

    } catch (const MonitorException& e) {
        // Something went badly wrong inside our own code
        Logger::getInstance()->logError(string("FATAL MonitorException: ") + e.what());
        return 1;
    } catch (const exception& e) {
        // Catch anything else we didn't anticipate (bad_alloc, etc.)
        Logger::getInstance()->logError(string("FATAL std::exception: ") + e.what());
        return 1;
    }
}