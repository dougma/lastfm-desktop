

#ifndef UNICORN_SETTINGS_H
#define UNICORN_SETTINGS_H

#include "UnicornDllExportMacro.h"
#include <QSettings>
#include <QString>


namespace Unicorn
{
    /** Use this if you need to store or access settings stored in the 
      * AudioScrobbler namespace
      * TODO should use Software/Last.fm on Windows
      *      fm.last.plist on mac
      *      .config/Last.fm on Linux
      */
    class QSettings : public ::QSettings
    {
    public:
        QSettings() : ::QSettings( "Last.fm", "AudioScrobbler" )
        {}
    };

    /** Settings that may be of use to the entire Last.fm suite 
      */
    class UNICORN_DLLEXPORT Settings
    {
    public:
        Settings();

        QString username() const { return QSettings().value( "Username" ).toString(); }
        QString password() const { return QSettings().value( "Password" ).toString(); }

    #if 0
        //TODO there is only one widget that can set the password
        // enforce that by moving this function there
    
        void setPassword( QString password )
        {
            if ( !password.isEmpty() && password != "********" )
            {
                password = UnicornUtils::md5Digest( password.toUtf8() );
                MyQSettings( this ).setValue( "Password", password );
            }
        }

        //TODO these should only be handled by same above metioned widget
        bool rememberPass() const
        { 
            // Written as int for backwards compatibility with the MFC Audioscrobbler
            return (bool)MyQSettings( this ).value( "RememberPass", true ).toInt();
        }

        void setRememberPass( bool remember )
        {
            MyQSettings( this ).setValue( "RememberPass", int(remember) );
            emit userChanged( username() );
        }
    #endif

        bool isUseProxy() const { return QSettings().value( "ProxyEnabled" ).toInt() == 1; }
        QString proxyHost() const { return QSettings().value( "ProxyHost" ).toString(); }
        int proxyPort() const { return QSettings().value( "ProxyPort" ).toInt(); }
        QString proxyUser() const { return QSettings().value( "ProxyUser" ).toString(); }
        QString proxyPassword() const { return QSettings().value( "ProxyPassword" ).toString(); }

        /** @returns one of our pre-defined 2-letter language codes */
        QString appLanguage() const;
    };

    //TODO make most of this private, friend to Settings dialog
    class MutableSettings : private Settings
    {
    public:
        MutableSettings()
        {}

        void setUseProxy( bool v ) { QSettings().setValue( "ProxyEnabled", v ? "1" : "0" ); }
        void setProxyHost( QString v ) { QSettings().setValue( "ProxyHost", v ); }
        void setProxyPort( int v ) { QSettings().setValue( "ProxyPort", v ); }
        void setProxyUser( QString v ) { QSettings().setValue( "ProxyUser", v ); }
        void setProxyPassword( QString v ) { QSettings().setValue( "ProxyPassword", v ); }
        void setAppLanguage( QString langCode ) { QSettings().setValue( "AppLanguage", langCode ); }
    };
}

#endif
