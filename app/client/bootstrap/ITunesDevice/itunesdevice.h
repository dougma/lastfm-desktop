/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

   This file is part of the Last.fm Desktop Application Suite.

   lastfm-desktop is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   lastfm-desktop is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with lastfm-desktop.  If not, see <http://www.gnu.org/licenses/>.
*/
/** @author Christian Muehlhaeuser <chris@last.fm>
  * @contributor Erik Jaelevik <erik@last.fm>
  * @contributor Max Howell <max@last.fm>
  */

#include <lastfm/Track>


class ITunesDevice : public QObject
{
    Q_OBJECT

public:
    ITunesDevice();

    QString LibraryPath();

    Track firstTrack( const QString& file );
    Track nextTrack();

signals:
    void progress( int percentage, const Track& track );

private:
    QString m_iTunesLibraryPath;
    QString m_database;

    class QFile* m_file;
    class ITunesParser* m_handler;
    int m_totalSize;
    class QXmlSimpleReader* m_xmlReader;
    class QXmlInputSource* m_xmlInput;
};
