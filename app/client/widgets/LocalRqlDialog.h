#include "ui_LocalRqlDialog.h"
#include <QDialog>
#include "app/clientplugins/ILocalRql.h"


class LocalRqlDialog : public QDialog, ILocalRqlParseCallback
{
    Q_OBJECT

    Ui::LocalRqlDialog ui;

    // ILocalRqlParseCallback
    void parseOk(class ILocalRqlTrackSource*);
    void parseFail(int errorLineNumber, const char *errorLine, int errorOffset);

public:
    LocalRqlDialog( QWidget *parent );

private slots:
    void accept();

};
