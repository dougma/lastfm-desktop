/***************************************************************************
 *   Copyright 2004 - 2008 by                                              *
 *      Gareth Simpson <iscrobbler@xurble.org>                             *
 *      Last.fm Ltd. <client@last.fm>                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#include "main.h"
#include "IPodDetector.h"
#include "Moose.h"
#include "common/c++/logger.h"

#if TARGET_OS_WIN32
    #include "ITunesComThread.h"
    #include "ScrobSubmitter.h"

    ITunesComThread* gCom = 0;
    ScrobSubmitter gSubmitter;
#else
    #include <sys/stat.h>
	#include "ITunesPlaysDatabase.h"
#endif

IPodDetector* gIPodDetector = 0;

static void initLogger();
static bool isClientInstalled();
static void launchClient();
static void setupIPodSystem();
static void cleanup();
static void uninstall();
static void deleteTwiddlyDatabases();
static OSStatus RegisterVisualPlugin( PluginMessageInfo* );


extern "C"
{
    IMPEXP OSStatus
    MAIN( OSType message, PluginMessageInfo* messageInfo, void* /*refCon*/ )
    {
        switch ( message )
        {
            case kPluginInitMessage:
                initLogger();

              #if TARGET_OS_MAC
                if ( !isClientInstalled() )
                {
                    uninstall();
                    return noErr;
                }
              #endif
                
                launchClient(); //launches client, on mac
                setupIPodSystem(); //sets up iPod scrobbling

              #if TARGET_OS_WIN32
                LOG( 3, "Initialising ScrobSubmitter" );
                gSubmitter.Init( "itw", ScrobSubCallback, 0 );
              #endif
                
                return RegisterVisualPlugin( messageInfo );

        
            case kPluginPrepareToQuitMessage:
                LOG( 3, "EVENT: kPluginPrepareToQuitMessage" );

                cleanup();
                return noErr;

        
            case kPluginCleanupMessage:
                LOG( 3, "EVENT: kPluginCleanupMessage" );

              #if TARGET_OS_WIN32
                LOG( 3, "Terminating ScrobSubmitter" );
                gSubmitter.Term();
              #endif

                return noErr;

        
            default:
                return unimpErr;
        }
    }
} // extern "C"


static void initLogger()
{
#if TARGET_OS_WIN32
    std::wstring path = ScrobSubmitter::GetLogPathW();
    path += L"\\iTunesPlugin.log";
#else
    std::string path = ::getenv( "HOME" );
    path += "/Library/Logs/Last.fm/iTunes.log";
#endif

    new Logger( path.c_str(), Logger::Debug );
    LOG( 3, "Log file created" );
}


static void launchClient()
{
#if TARGET_OS_MAC
    if ( Moose::launchWithMediaPlayer() )
    {
        Moose::exec( Moose::applicationFolder() + "Last.fm", "--tray" );
    }
#endif
}


static void setupIPodSystem()
{
    if ( !Moose::iPodScrobblingEnabled() )
    {
        deleteTwiddlyDatabases();
        return;
    }

#if TARGET_OS_WIN32
    LOG( 3, "Initialising ITunesComThread" );
    gCom = new ITunesComThread();
#endif

#if TARGET_OS_MAC
    LOG( 3, "Initialising ITunesPlaysDatabase" );
    ITunesPlaysDatabase::init(); // must be done b4 notification center registration

    CFNotificationCenterAddObserver( CFNotificationCenterGetDistributedCenter(),
                                     NULL,
                                     notificationCallback,
                                     CFSTR( "com.apple.iTunes.playerInfo" ),
                                     NULL,
                                     CFNotificationSuspensionBehaviorDeliverImmediately );
#endif

    LOG( 3, "Initialising IPodDetector" );
    gIPodDetector = new IPodDetector;
}


static void cleanup()
{
    LOG( 3, "Deleting IPodDetector" );
    delete gIPodDetector;
    gIPodDetector = 0;
    
#if TARGET_OS_WIN32
    LOG( 3, "Deleting ITunesComThread" );
    delete gCom;
    gCom = 0;
#else
    ITunesPlaysDatabase::finit();
#endif
}


#if TARGET_OS_MAC
static bool isClientInstalled()
{
    struct stat st;
    std::string path = Moose::applicationFolder() + "Last.fm";
    return stat( path.c_str(), &st ) == 0;
}


static void uninstall()
{
    LOG( 3, "Client has gone, uninstalling self." );
    // we have to change directory for the script to work properly
    std::string d = Moose::bundleFolder() + "Contents/Resources";
    // we have to change dir for the script to succeed as a safety precaution
    if (chdir( d.c_str() ) != 0)
        return;
    // always specify paths to tools absolutely! --muesli
    Moose::exec( "/bin/sh", "uninstall.sh" );
}
#endif


static void deleteTwiddlyDatabases()
{
    // we delete the db, or if they ever re-enable iPod scrobbling we will
    // mis-scrobble everything played between now and then
    #if TARGET_OS_WIN32
        //FIXME you prolly should delete the manual iPod databases too
        DeleteFileW( (Moose::applicationSupport() + L"\\iTunesPlays.db").c_str() );
    #else
        std::string script = Moose::bundleFolder() + "Contents/Resources/uninstall.sh";
        Moose::exec( "/bin/sh", "'" + script + "' --db-only" );
    #endif
}


static OSStatus
RegisterVisualPlugin( PluginMessageInfo *messageInfo )
{
    OSStatus status;
    PlayerMessageInfo playerMessageInfo;

#if TARGET_OS_WIN32
    ClearMemory( &playerMessageInfo.u.registerVisualPluginMessage, 
                 sizeof( playerMessageInfo.u.registerVisualPluginMessage ) );
#else
    memset( &playerMessageInfo.u.registerVisualPluginMessage, 
            0,
            sizeof( playerMessageInfo.u.registerVisualPluginMessage ) );
#endif

#if TARGET_OS_MAC
    const char* pluginName = kVisualPluginName;
    int const n = strlen( pluginName );
    playerMessageInfo.u.registerVisualPluginMessage.name[0] = n;
    memcpy( &playerMessageInfo.u.registerVisualPluginMessage.name[1], &pluginName[0], n );
#else
    // copy in name length byte first
    playerMessageInfo.u.registerVisualPluginMessage.name[0] = lstrlenA( kVisualPluginName );

    // now copy in actual name
    memcpy( &playerMessageInfo.u.registerVisualPluginMessage.name[1],
            kVisualPluginName,
            lstrlenA(kVisualPluginName));
#endif

    SetNumVersion( &playerMessageInfo.u.registerVisualPluginMessage.pluginVersion,
        kVisualPluginMajorVersion,
        kVisualPluginMinorVersion,
        kVisualPluginReleaseStage,
        kVisualPluginNonFinalRelease );

    LOG( 3, "Giving iTunes version number: " + GetVersionString() );

#if TARGET_OS_WIN32
    //FIXME actually, I doubt we want these
    playerMessageInfo.u.registerVisualPluginMessage.options                 = kVisualWantsIdleMessages;
#endif
    playerMessageInfo.u.registerVisualPluginMessage.handler                 = VisualPluginHandler;
    playerMessageInfo.u.registerVisualPluginMessage.registerRefCon          = 0;
    playerMessageInfo.u.registerVisualPluginMessage.creator                 = kVisualPluginCreator;

    // following sets render to be called every 100ms
    playerMessageInfo.u.registerVisualPluginMessage.timeBetweenDataInMS     = 100; // 16 milliseconds = 1 Tick, 0xFFFFFFFF = Often as possible.
    playerMessageInfo.u.registerVisualPluginMessage.numWaveformChannels     = 2;
    playerMessageInfo.u.registerVisualPluginMessage.numSpectrumChannels     = 2;

    playerMessageInfo.u.registerVisualPluginMessage.minWidth                = 64;
    playerMessageInfo.u.registerVisualPluginMessage.minHeight               = 64;
    playerMessageInfo.u.registerVisualPluginMessage.maxWidth                = 32767;
    playerMessageInfo.u.registerVisualPluginMessage.maxHeight               = 32767;
    playerMessageInfo.u.registerVisualPluginMessage.minFullScreenBitDepth   = 0;
    playerMessageInfo.u.registerVisualPluginMessage.maxFullScreenBitDepth   = 0;
    playerMessageInfo.u.registerVisualPluginMessage.windowAlignmentInBytes  = 0;

    messageInfo->u.initMessage.options = 0;
    messageInfo->u.initMessage.refCon = 0;
    
    status = PlayerRegisterVisualPlugin( messageInfo->u.initMessage.appCookie,
                                         messageInfo->u.initMessage.appProc,
                                         &playerMessageInfo );
    return status;
}


std::string
GetVersionString()
{
    std::ostringstream os;
    os << kVisualPluginMajorVersion << "."
       << kVisualPluginMinorVersion << "."
       << kVisualPluginReleaseStage << "."
       << kVisualPluginNonFinalRelease;
    return os.str();
}
