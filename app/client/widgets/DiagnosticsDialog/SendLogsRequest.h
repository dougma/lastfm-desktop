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
#ifndef SEND_LOGS_REQUEST_H
#define SEND_LOGS_REQUEST_H

#include <QString>
#include <QStringList>
#include <QByteArray>
#include <lastfm/WsAccessManager>
#include <QNetworkReply>
#include "lib/DllExportMacro.h"


class SendLogsRequest : public QObject
{
    Q_OBJECT

    signals:
        void success();
        void error();

    public:
        SendLogsRequest( const QString& usernotes );
        
        void addLog( QString name, QString filename );
        void addLogData( QString name, QString data );
        
        void send();
        
    protected:
        QString escapeString( QString );
        QByteArray postData( QString name, QByteArray data );
        
        WsAccessManager m_networkAccessManager;
        QByteArray m_data;
        QStringList m_logs;
        QString m_usernotes;
        bool m_error;
    
    private slots:
        void onFinished();
        void onError( QNetworkReply::NetworkError code );
};

#endif

