/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd.                                      *
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
