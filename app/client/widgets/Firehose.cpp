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

#include "Firehose.h"
#include "lib/core/CoreDomElement.h"
#include "lib/ws/WsAccessManager.h"
#include <QDebug>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QWebView>
#include <QVBoxLayout>


Firehose::Firehose()
{
    setLayout( new QVBoxLayout );
    
    m_nam = new WsAccessManager( this );
    
    QUrl url( "http://87.117.229.70:8001/firehose/user/1000002?rt=xml&callback=none&special=staffmembers" );
    QNetworkRequest request( url );
    
    // set to RJ's user id for the moment
    QNetworkReply* r = m_nam->get( request );
    connect( r, SIGNAL(readyRead()), SLOT(onData()) );
    connect( r, SIGNAL(finished()), SLOT(onFinished()) );
}


void
Firehose::onData()
{
    QNetworkReply* r = (QNetworkReply*)sender();
    QByteArray data = r->readAll();
    
    // RJ's XML is malformed.. :(
    QString massaged_data = data;
    massaged_data.remove( QRegExp( "<album>.*</album>" ) );
    
    QDomDocument xml;
    xml.setContent( massaged_data );

    try
    {
        QString html;
        
        CoreDomElement e( xml.documentElement() );
        
        html += "<a href='" + e["user"]["url"].text() + "'><img src='" + e["user"]["image"].text() + "'>" + e["user"]["name"].text() + "</a><br>";
        html += e["track"]["artist"]["name"].text() + " &ndash; " + e["track"]["name"].text();
        
        QWebView* web = new QWebView;
        layout()->addWidget( web );
        web->setHtml( html );
    }
    catch (CoreDomElement::Exception& e)
    {
        qWarning() << e;
        qDebug() << data;
    }
}


void
Firehose::onFinished()
{
    qDebug() << "Oh, we d/c'd. Prolly an error happened";
}