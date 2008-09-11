
#include "Growl.h"
#include "../AppleScript.h"
#include "../Unicornutils.h"
#include <QCoreApplication>
#include <QFileInfo>


Growl::Growl( const QString& name )
     : m_name( name )
{}


void
Growl::notify()
{
    if (!Unicorn::isProcessRunning( "GrowlHelperApp" ))
        return;

    AppleScript script;
    script << "tell application 'GrowlHelperApp'"
           <<     "register as application '" + qApp->applicationName() + "'"
                          " all notifications {'" + m_name + "'}"
                          " default notifications {'" + m_name + "'}"
                          " icon of application 'Last.fm.app'"
           <<     "notify with name '" + m_name + "'"
                          " title " + AppleScript::asUnicodeText( m_title ) +
                          " description " + AppleScript::asUnicodeText( m_description ) + 
                          " application name '" + qApp->applicationName() + "'"
           << "end tell";
    script.exec();
}
