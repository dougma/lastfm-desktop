#include "ui_LocalRqlDialog.h"
#include <QDialog>
#include "app/clientplugins/ILocalRql.h"


class LocalRqlDialog : public QDialog
{
    Q_OBJECT

    Ui::LocalRqlDialog ui;

public:
    LocalRqlDialog( QWidget *parent );

private slots:
    void onPlay();
    void onTag();

    void onParseGood(unsigned trackCount);
    void onParseBad(int errorLineNumber, QString errorLine, int errorOffset);
};
