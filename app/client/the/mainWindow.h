#include "../widgets/MainWindow.h"

namespace The
{
    MainWindow& mainWindow(); //defined in App.cpp
    inline QMainWindow* qMainWindow() { return &(QMainWindow&)mainWindow(); }
}
