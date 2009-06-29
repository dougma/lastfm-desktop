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
#include "MainWidget.h"
#include <lastfm.h>
#include <QtGui>
#include <stdarg.h>
#include "layouts/SideBySideLayout.h"
#include "widgets/MainStarterWidget.h"
#include "widgets/NowPlayingWidget.h"
#include "widgets/MultiStarterWidget.h"

MainWidget::MainWidget()
{
    m_layout = new SideBySideLayout;

    MainStarterWidget* w = new MainStarterWidget;
    connect(w, SIGNAL(startRadio(RadioStation)), SIGNAL(startRadio(RadioStation)));
    connect(w, SIGNAL(startRadio(RadioStation)), SLOT(onStartRadio(RadioStation)));
    connect(w, SIGNAL(combo()), SLOT(onCombo()));
    connect(w, SIGNAL(yourTags()), SLOT(onYourTags()));

    m_layout->addWidget(w);

    setLayout(m_layout);
}

void 
MainWidget::onStartRadio(RadioStation rs)
{
    NowPlayingWidget* n = new NowPlayingWidget;
    m_layout->addWidget(n);
    m_layout->moveForward();
}

void
MainWidget::onCombo()
{
    MultiStarterWidget* multi = new MultiStarterWidget(3);
    connect(multi, SIGNAL(startRadio()), SIGNAL(startRadio()));
    connect(multi, SIGNAL(startRadio(RadioStation)), SLOT(onStartRadio(RadioStation)));
    BackWrapper* back = new BackWrapper(tr("Back"), multi);
    connect(back, SIGNAL(back()), SLOT(onBack()));
    m_layout->addWidget(back);
    m_layout->moveForward();
}

void
MainWidget::onBack()
{
    QWidget* w = m_layout->currentWidget();
    m_layout->moveBackward();
    m_layout->removeWidget(w);
    w->deleteLater();
}

void
MainWidget::onYourTags()
{
    qDebug() << "todo";
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
