/***************************************************************************
 *   Copyright 2008 Last.fm Ltd.                                           *
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

#include "ITunesLibrary.h"
#include "IPodScrobble.h"
#include "IPodSettings.h"
#include "PlayCountsDatabase.h"
#include <QDir>
#include <QDomDocument>
#include <QStringList>


/** @author <max@last.fm>
  */
class IPod
{
public:   
    virtual ~IPod()
    {}
    
    /** you own the memory */
    static IPod* fromCommandLineArguments( const QStringList& );
    
    /** figures out the iPod scrobbles */
    void twiddle();

    /** allows us to encapsulate the real scrobble count() */
    class ScrobbleList : private QList<Track>
    {
        int m_realCount;

    public:
        ScrobbleList() : m_realCount( 0 )
        {}
        using QList<Track>::isEmpty;
        QDomDocument xml() const;
        int count() const { return m_realCount; }
        ScrobbleList& operator+=( const Track& t )
        {
            m_realCount += IPodScrobble(t).playCount();
            append( t );
            return *this;
        }
    #ifdef WIN32
        /** verbose, but named to make it clear you're expected to set uniqueIDs
          * on tracks added to the ScrobbleList on Windows, at least if you want
          * to remove them */
        void removeAllWithUniqueId( const QString& uniqueId );
    #endif
        void clear()
        {
            m_realCount = 0;
            QList<Track>::clear();
        }
    };
    
    /** this must be writable by Qt as a path on all platforms */
    QString uid() const { return device + '/' + serial; }

    ScrobbleList scrobbles() const { return m_scrobbles; }
    ScrobbleList& scrobbles() { return m_scrobbles; } //so we can clear()

    IPodSettings settings() const 
	{
		return IPodSettings( device + '/' + serial );
	}

    /** assigned to the mediaDeviceId on TrackInfo objects */
    QString scrobbleId() const;
    
    /** every device has its own directory for storing stuff */
    QDir saveDir() const;

    QString device;
    QString vid;
    QString pid; //product id, not persistent id
    QString serial;

protected:    
    ScrobbleList m_scrobbles;

    /** heap allocate and return those relevent to your iPod type */
    virtual class PlayCountsDatabase* playCountsDatabase() = 0;
    virtual class ITunesLibrary* iTunesLibrary() = 0;

};



class AutomaticIPod : public IPod
{
public:
    class PlayCountsDatabase : public ::PlayCountsDatabase
    {
    public:
        PlayCountsDatabase();

        /** duplicates the contents of the iTunes Library into our database */
        void bootstrap();
        bool isBootstrapNeeded() const;
    };

private:
    virtual PlayCountsDatabase* playCountsDatabase() { return new PlayCountsDatabase; }
    virtual ITunesLibrary* iTunesLibrary() { return new ITunesLibrary; }
};



class ManualIPod : public IPod
{
public:
    ManualIPod();

    class PlayCountsDatabase : public ::PlayCountsDatabase
    {
    public:
        PlayCountsDatabase( class IPod const * const ipod )
                : ::PlayCountsDatabase( ipod->saveDir().filePath( "playcounts.db" ) )
        {}
    };

    class Library : public ITunesLibrary
    {
    public:
        Library( const QString& pid ) : ITunesLibrary( pid, true )
        {}
    };

private:
    virtual PlayCountsDatabase* playCountsDatabase() { return new PlayCountsDatabase( this ); }
    virtual ITunesLibrary* iTunesLibrary() { return new Library( m_pid ); }

    /** persistent ID of the iPod source, mac only */
    QString const m_pid;

  #ifdef Q_OS_MAC    
    static QString firstPid();
  #endif
};
