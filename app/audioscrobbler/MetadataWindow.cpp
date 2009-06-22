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
#include "MetadataWindow.h"
#include "lib/unicorn/widgets/ImageButton.h"
#include <lastfm/Artist>
#include <lastfm/XmlQuery>
#include <lastfm/ws.h>
#include <QLabel>
#include <QPainter>
#include <QTextBrowser>
#include <QNetworkReply>
#include <QTextFrame>
#include <QTextFrameFormat>
#include <QVBoxLayout>
#include "Application.h"

static inline ImageButton* ib(const char* s)
{
    ImageButton* b = new ImageButton(s);
    b->setFixedSize(36, 36);
    return b;
}

MetadataWindow::MetadataWindow()
{
    setCentralWidget(new QWidget);

    QHBoxLayout* h = new QHBoxLayout;
    h->addStretch();
    
    h->addWidget(ui.love = ib(":love.png"));
    ui.love->setAction( ((audioscrobbler::Application*)qApp)->loveAction() );
    
    h->addWidget(ui.tag = ib(":tag.png"));
    ui.tag->setAction( ((audioscrobbler::Application*)qApp)->tagAction() );
    
    h->addWidget(ui.share = ib(":share.png"));
    ui.share->setAction( ((audioscrobbler::Application*)qApp)->shareAction() );
    
    h->addStretch();

    QVBoxLayout* v = new QVBoxLayout(centralWidget());
    v->addWidget(ui.artist_image = new QLabel);
    v->addWidget(ui.bio = new QTextBrowser);
    v->addLayout(h);
    v->setStretchFactor(ui.bio, 1);
    v->setSpacing(0);
    v->setMargin(0);

    v = new QVBoxLayout(ui.artist_image);
    v->addStretch();
    v->addWidget(ui.title = new QLabel);

    QFont f = ui.title->font();
    f.setBold(true);
    ui.title->setFont(f);

    QPalette p;
    QColor gray(28, 28, 28);
    p.setColor(QPalette::Text, Qt::white);
    p.setColor(QPalette::Base, gray);
    p.setColor(QPalette::WindowText, Qt::white);
    p.setColor(QPalette::Window, gray);

    ui.title->setPalette(p);
    ui.bio->setPalette(p);

    ui.bio->setFrameStyle(QFrame::NoFrame);
    ui.bio->viewport()->setContentsMargins(12, 12, 12, 12);

    setFixedWidth(252);
    setWindowTitle(tr("Last.fm Audioscrobbler"));
    resize(20, 500);
}

void
MetadataWindow::onTrackStarted(const Track& t, const Track& previous)
{
    QString title="%1\n%2 (%3)";
    ui.title->setText(title.arg(t.artist()).arg(t.title()).arg(t.durationString()));

    if (t.artist() == previous.artist())return;

    ui.bio->clear();
    ui.artist_image->clear();
    m_currentTrack = t;
    connect(t.artist().getInfo(), SIGNAL(finished()), SLOT(onArtistGotInfo()));
//    connect(t.album().getInfo(), SIGNAL(finished()), SLOT(onAlbumGotInfo()));
}

void
MetadataWindow::onArtistGotInfo()
{
    XmlQuery lfm = static_cast<QNetworkReply*>(sender())->readAll();

    //TODO if empty suggest they edit it
    ui.bio->setHtml(lfm["artist"]["bio"]["content"].text());

    QTextFrame* root = ui.bio->document()->rootFrame();
    QTextFrameFormat f = root->frameFormat();
    f.setMargin(12);
    root->setFrameFormat(f);


    QUrl url = lfm["artist"]["image size=large"].text().replace("/126/", "/252/");
    QNetworkReply* reply = lastfm::nam()->get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), SLOT(onArtistImageDownloaded()));
}

void
MetadataWindow::onArtistImageDownloaded()
{
    QPixmap px;
    px.loadFromData(static_cast<QNetworkReply*>(sender())->readAll());

    QLinearGradient g(QPoint(), px.rect().bottomLeft());
    g.setColorAt( 0.0, QColor(0, 0, 0, 0.11*255));
    g.setColorAt( 1.0, QColor(0, 0, 0, 0.88*255));

    QPainter p(&px);
    p.setCompositionMode(QPainter::CompositionMode_Multiply);
    p.fillRect(px.rect(), g);
    p.end();

    ui.artist_image->setPixmap(px);
}

void
MetadataWindow::onStopped()
{
    ui.bio->clear();
    ui.artist_image->clear();
    ui.title->clear();
    m_currentTrack = Track();
}
