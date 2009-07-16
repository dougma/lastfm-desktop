#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "lib/unicorn/UnicornMainWindow.h"
#include <lastfm/RadioStation>

class MainWindow : public unicorn::MainWindow
{
    Q_OBJECT
public:
    MainWindow();

signals:
    void startRadio( const RadioStation& );
};

#endif // MAINWINDOW_H
