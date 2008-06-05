
#ifndef AUDIOSCROBBLER_SETTINGS_H
#define AUDIOSCROBBLER_SETTINGS_H

#include "lib/moose/Settings.h"

int main( int, char** );
class Settings;
namespace The { Settings& settings(); }


class Settings : public Moose::Settings
{
    Settings( const QString& version, const QString& path );

    static Settings* instance;
    friend int main( int, char** );
    friend Settings& The::settings();

public:
    QByteArray containerGeometry() const { return QSettings().value( "MainWindowGeometry" ).toByteArray(); }
    Qt::WindowState containerWindowState() const { return (Qt::WindowState) QSettings().value( "MainWindowState" ).toInt(); }

private:
    bool m_weWereJustUpgraded;
};


class MutableSettings : private Settings
{
public:
    MutableSettings();

    void setControlPort( int v ) { QSettings().setValue( "ControlPort", v ); }
    void setScrobblePoint( int scrobblePoint ) { QSettings().setValue( "ScrobblePoint", scrobblePoint ); }
    void setContainerWindowState( int state ) { QSettings().setValue( "MainWindowState", state ); }
    void setContainerGeometry( QByteArray state ) { QSettings().setValue( "MainWindowGeometry", state ); }
};


namespace The
{
    inline Settings& settings()
    {
        return *Settings::instance;
    }
}

#endif
