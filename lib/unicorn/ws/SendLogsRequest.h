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

#ifndef SEND_LOGS_REQUEST_H
#define SEND_LOGS_REQUEST_H

#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class SendLogsRequest : public QObject
{
    Q_OBJECT

    signals:
        void error();

    public:
        SendLogsRequest(QString clientname, QString clientversion, QString usernotes );
        
        void addLog( QString name, QString filename );
        void addLogData( QString name, QString data );
        
        void send();
        
    protected:
        QString escapeString( QString );
        QByteArray postData( QString name, QByteArray data );
        
        QNetworkAccessManager m_networkAccessManager; //FIXME: this should be shared
        QByteArray m_data;
        QStringList m_logs;
        QString m_clientname, m_clientversion, m_usernotes;
    
    private slots:
        void onFinished();
        void onError( QNetworkReply::NetworkError code );
};

#endif

