#include "LocalRqlDialog.h"
#include "PluginHost.h"
#include "LocalRadioTrackSource.h"
#include "the/app.h"
#include "lib/unicorn/QMessageBoxBuilder.h"


LocalRqlDialog::LocalRqlDialog( QWidget *parent )
: QDialog( parent )
{
    ui.setupUi( this );
    connect(ui.playButton, SIGNAL(clicked()), this, SLOT(onPlay()));
    connect(ui.tagButton, SIGNAL(clicked()), this, SLOT(onTag()));
}

void LocalRqlDialog::onPlay()
{
#ifndef NDEBUG
    QString plugins_path = qApp->applicationDirPath();
#else
#ifdef Q_WS_X11
    QString plugins_path = "/usr/lib/lastfm/";
#else
    QString plugins_path = qApp->applicationDirPath() + "/plugins";
#endif
#endif

    PluginHost ph(plugins_path);
    QList<ILocalRqlPlugin *> plugins = ph.getPlugins<ILocalRqlPlugin>("LocalRql");
    if (plugins.empty())
        return;

    QString s = ui.lineEditRql->text();

    plugins[0]->init();
    plugins[0]->parse(s.toUtf8(), this);
}

void 
LocalRqlDialog::parseOk(ILocalRqlTrackSource* rqlSource)
{
    Q_ASSERT(rqlSource);
    if (rqlSource) {
        LocalRadioTrackSource* lrts = new LocalRadioTrackSource(rqlSource);
        The::app().openLocalContent(lrts);
        lrts->start();
    }
}

void 
LocalRqlDialog::parseFail(int errorLineNumber, const char *errorLine, int errorOffset)
{
    QMessageBox::critical(this, "RQL parse error", errorLine, QMessageBox::Cancel, QMessageBox::Cancel);
}

void
LocalRqlDialog::onTag()
{
#ifndef NDEBUG
    QString plugins_path = qApp->applicationDirPath();
#else
#ifdef Q_WS_X11
    QString plugins_path = "/usr/lib/lastfm/";
#else
    QString plugins_path = qApp->applicationDirPath() + "/plugins";
#endif
#endif

    PluginHost ph(plugins_path);
    QList<ILocalRqlPlugin *> plugins = ph.getPlugins<ILocalRqlPlugin>("LocalRql");
    if (plugins.empty())
        return;

    QString s = ui.lineEditTagifierUrl->text();

    plugins[0]->init();
    plugins[0]->testTag(s.toUtf8());
 
}