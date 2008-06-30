/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd.                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *    This program is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#include <QObject>
 
namespace Phonon
{
    class MediaObject;
    class AudioOutput;
    class MediaSource;
}


class RadioPlayer : public QObject
{
    Q_OBJECT
    
public:
    RadioPlayer( const QString& username, const QString& username );
    
public slots:
    void play( const QString& lastfm_url );
    void skip();
    void stop();

private slots:
    void onTracksReady();
    void onAboutToFinishPlaylist();

private:
    class Radio* m_radio;
    Phonon::MediaObject *m_mediaObject;
    Phonon::AudioOutput *m_audioOutput;
};
