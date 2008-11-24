#include "ui_LocalRqlDialog.h"
#include <QDialog>

class LocalRqlDialog : public QDialog
{
    Q_OBJECT

    Ui::LocalRqlDialog ui;

public:
    LocalRqlDialog( QWidget *parent );

private slots:
    void accept();

};
