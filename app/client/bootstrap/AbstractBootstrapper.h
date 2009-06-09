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
#ifndef ABSTRACT_BOOTSTRAPPER_H
#define ABSTRACT_BOOTSTRAPPER_H

#include <QObject>


class AbstractBootstrapper : public QObject
{
    Q_OBJECT

public:
    enum BootstrapStatus
    {
        Bootstrap_Ok = 0,
        Bootstrap_UploadError,
        Bootstrap_Denied,
        Bootstrap_Spam /* eg. 1 billion plays for Bjork */
    };

    AbstractBootstrapper( QObject* parent = NULL );

    bool zipFiles( const QString& inFileName, const QString& outFileName ) const;
    void sendZip( const QString& inFile );

    virtual void bootStrap() = 0;

signals:
    void percentageUploaded( int );
    void done( int /* BootstrapStatus */ status );

protected slots:
    void onUploadDone( int id, bool error );
    void onUploadProgress( int done, int total );

private:
    class QHttp* m_http;
    int m_reqId;
};

#endif
