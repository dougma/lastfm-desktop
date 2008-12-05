#include "LocalRqlDialog.h"
#include "LocalRql.h"
#include "LocalRadioTrackSource.h"
#include "the/app.h"
#include "lib/unicorn/QMessageBoxBuilder.h"


LocalRqlDialog::LocalRqlDialog( QWidget *parent )
: QDialog( parent )
{
    ui.setupUi( this );
    
    if (The::app().localRql()->isAvailable()) {
        connect(ui.playButton, SIGNAL(clicked()), this, SLOT(onPlay()));
        connect(ui.tagButton, SIGNAL(clicked()), this, SLOT(onTag()));
    } else {
        ui.playButton->setDisabled(true);
        ui.tagButton->setDisabled(true);
    }
}

void LocalRqlDialog::onPlay()
{
    QString rql = ui.lineEditRql->text();
    LocalRqlResult* rqlResult = The::app().localRql()->startParse(rql);
    if (rqlResult) {
        // note: QueuedConnection is important here:
        connect(rqlResult, SIGNAL(parseGood(unsigned)), SLOT(onParseGood(unsigned)), Qt::QueuedConnection);
        connect(rqlResult, SIGNAL(parseBad(int,QString,int)), SLOT(onParseBad(int,QString,int)), Qt::QueuedConnection);
    }
}

void 
LocalRqlDialog::onParseGood(unsigned trackCount)
{
    Q_UNUSED(trackCount);

    LocalRqlResult* rqlResult = dynamic_cast<LocalRqlResult*>(sender());
    if (rqlResult) {
        LocalRadioTrackSource* lrts = new LocalRadioTrackSource(rqlResult);
        The::app().openLocalContent(lrts);
        lrts->start();
    }
}

void 
LocalRqlDialog::onParseBad(int errorLineNumber, QString errorLine, int errorOffset)
{
    Q_UNUSED( errorLineNumber );
    Q_UNUSED( errorOffset );
    
    QMessageBox::critical(this, "RQL parse error", errorLine, QMessageBox::Cancel, QMessageBox::Cancel);
}

void
LocalRqlDialog::onTag()
{
    QString url = ui.lineEditTagifierUrl->text();
    The::app().localRql()->testTag(url);
}
