#include "LocalRqlDialog.h"
#include "PluginHost.h"
#include "the/app.h"


LocalRqlDialog::LocalRqlDialog( QWidget *parent )
: QDialog( parent )
{
    ui.setupUi( this );
}

void LocalRqlDialog::accept()
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

    QString s = ui.lineEdit->text();

    plugins[0]->init();
    plugins[0]->parse(s.toUtf8(), this);
}

void 
LocalRqlDialog::parseOk(ILocalRqlTrackSource* trackSource)
{
    Q_ASSERT(trackSource);
    if (trackSource) {
//        The::app().playLocal(trackSource);
    }
}

void 
LocalRqlDialog::parseFail(int errorLineNumber, const char *errorLine, int errorOffset)
{
    int ii = 0;
}