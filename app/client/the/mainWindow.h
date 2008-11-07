#ifndef THE_MAINWINDOW_H
#define THE_MAINWINDOW_H

#include "../widgets/MainWindow.h"

namespace The
{
    MainWindow& mainWindow(); //defined in App.cpp
    inline QMainWindow* qMainWindow() { return &(QMainWindow&)mainWindow(); }
}

#endif //THE_MAINWINDOW_H