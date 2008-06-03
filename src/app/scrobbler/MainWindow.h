#include <QMainWindow>

#include "ui_MainWindow.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

    Ui::MainWindow ui;

public:
    MainWindow();

public slots:
    void onTrackChanged( const class TrackInfo& );
};
