#include "LocalRqlDialog.h"
#include "PluginHost.h"
#include "../clientplugins/ILocalRql.h"

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
    plugins[0]->play(s.toUtf8());

}