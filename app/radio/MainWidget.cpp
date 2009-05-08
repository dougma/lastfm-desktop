/***************************************************************************
 *   Copyright 2005-2009 Last.fm Ltd.                                      *
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

#include "MainWidget.h"
#include <lastfm.h>
#include <QtGui>
#include <stdarg.h>


MainWidget::MainWidget()
{
    QGridLayout* g = new QGridLayout(this);
    g->addWidget(ui.friends = new FriendsList, 0, 0);
    g->addWidget(ui.neighbours = new NeighboursList, 0, 1);
    g->addWidget(ui.me = new Me, 0, 2);
    g->addWidget(ui.friends_count = new QLabel, 1, 0);
    g->addWidget(ui.neighbour_tags = new QLabel, 1, 1);
    g->addWidget(ui.scrobbles = new QLabel, 1, 2);
    g->setSpacing(0);
    g->setMargin(0);
    g->setColumnStretch(0, 1);
    g->setColumnStretch(1, 1);
    g->setColumnStretch(2, 1);

    #define MACRO(x) x->setAlignment(Qt::AlignCenter); x->setFixedHeight(fontMetrics().height()+4);
    MACRO(ui.scrobbles)
    MACRO(ui.friends_count)
    MACRO(ui.neighbour_tags)
    connect(qApp, SIGNAL(userGotInfo( QNetworkReply* )), SLOT(onUserGotInfo( QNetworkReply* )));
    
    AuthenticatedUser you;
    connect(you.getFriends(), SIGNAL(finished()), SLOT(onUserGotFriends()));
    connect(you.getNeighbours(), SIGNAL(finished()), SLOT(onUserGotNeighbours()));
    connect(you.getTopTags(), SIGNAL(finished()), SLOT(onUserGotTopTags()));
}


QString magic(XmlQuery e, ...)
{
    qDebug() << "sup";
    QString out;
    va_list ap;
    va_start(ap, e);
    while(const char* args = va_arg(ap, const char*)){
        qDebug() << args;
        QString const arg = e[args].text();

        if(arg.size()){
            out += QString(va_arg(ap, const char*)).arg(arg);
            out += "\n";
    }}
    va_end(ap);
    return out;
}


void
MainWidget::onUserGotInfo(QNetworkReply* r)
{
    XmlQuery e = XmlQuery(r->readAll())["user"];
    uint count = e["playcount"].text().toUInt();
    ui.scrobbles->setText(tr("%L1 scrobbles").arg(count));
#if 0
    QString s = magic(e,
                      "name", "<h1>%1</h1>",
                      "image", "<img src='%1'>",
                      "age", "%L1 years old",
                      "country", "From %1",
                      0);
    ui.me->setText(s);
#endif
}

void
MainWidget::onUserGotFriends()
{
    QNetworkReply* r = (QNetworkReply*)sender();
    XmlQuery lfm = r->readAll();
    
    uint count = 0; //TODO count is wrong as webservice is paginated
    foreach (XmlQuery e, lfm["friends"].children("user"))
    {
        count++;
        ui.friends->addItem(e["name"].text());
    }
    ui.friends_count->setText(tr("%L1 friends").arg(count));
}


void
MainWidget::onUserGotNeighbours()
{
    QNetworkReply* r = (QNetworkReply*)sender();
    XmlQuery lfm = r->readAll();

    foreach (XmlQuery e, lfm["neighbours"].children("user"))
    {
        ui.neighbours->addItem(e["name"].text());
    }
}

void
MainWidget::onUserGotTopTags()
{
    QNetworkReply* r = (QNetworkReply*)sender();
    XmlQuery lfm = r->readAll();

    QStringList tags;
    uint x = 0;
    foreach (XmlQuery e, lfm["toptags"].children("tag"))
    {
        if(++x == 3) break;
        tags += e["name"].text();
    }
    ui.neighbour_tags->setText(tags.join(", "));
}


FriendsList::FriendsList()
{}


NeighboursList::NeighboursList()
{}


Me::Me()
{}
