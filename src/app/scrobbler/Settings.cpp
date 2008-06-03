
#include "Settings.h"

Settings* Settings::instance = 0;


Settings::Settings( const QString& version, const QString& path ) 
{
    Q_ASSERT( !instance );

    instance = this;

    QSettings s;
    m_weWereJustUpgraded = version != s.value( "Version", "An Impossible Version String" );
    
    s.setValue( "Path", path );
    s.setValue( "Version", version ); 
}
