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

#include "ScrobbleStatus.h"
#include "ScrobbleControls.h"
#include "Application.h"
#include "lib/unicorn/StylableWidget.h"

#include <lastfm/Artist>
#include <lastfm/XmlQuery>
#include <lastfm/ws.h>

#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QTextBrowser>
#include <QNetworkReply>
#include <QTextFrame>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QStatusBar>
#include <QSizeGrip>
#include <QDesktopServices>
#include <QAbstractTextDocumentLayout>

MetadataWindow::MetadataWindow()
{
    setCentralWidget(new QWidget);
    QVBoxLayout* v = new QVBoxLayout(centralWidget());

    setMinimumWidth( 410 );

    v->addWidget(ui.now_playing_source = new ScrobbleStatus());
    ui.now_playing_source->setObjectName("now_playing");
    ui.now_playing_source->setFixedHeight( 22 );
    QVBoxLayout* vs;
    {
        QWidget* scrollWidget;
        QScrollArea* sa = new QScrollArea();
        sa->setWidgetResizable( true );
        sa->setWidget( scrollWidget = new StylableWidget(sa));
        sa->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
        vs = new QVBoxLayout( scrollWidget );
        v->addWidget( sa );
    }



    // listeners, scrobbles, tags:
    {
       
        QLabel* label;
        QGridLayout* grid = new QGridLayout();
        grid->setSpacing( 0 );

        {
            QVBoxLayout* v2 = new QVBoxLayout();
            grid->addWidget(ui.artist_image = new QLabel, 0, 0, Qt::AlignTop | Qt::AlignLeft );
            ui.artist_image->setObjectName("artist_image");
            v2->addWidget(ui.title = new QLabel);
            v2->addWidget(ui.album = new QLabel);
            v2->addStretch();
            ui.title->setObjectName("title1");
            ui.title->setTextInteractionFlags( Qt::TextSelectableByMouse );
            ui.title->setWordWrap(true);
            ui.album->setObjectName("title2");
            grid->addLayout(v2, 0, 1, Qt::AlignTop );
        }

        label = new QLabel(tr("Listeners"));
        label->setObjectName("name");
        label->setProperty("alternate", QVariant(true));
        label->setAlignment( Qt::AlignTop );
        grid->addWidget( label, 1, 0 );
        ui.listeners = new QLabel;
        ui.listeners->setObjectName("value");
        ui.listeners->setProperty("alternate", QVariant(true));
        grid->addWidget(ui.listeners, 1, 1);

        label = new QLabel(tr("Scrobbles"));
        label->setObjectName("name");
        label->setAlignment( Qt::AlignTop );
        grid->addWidget( label, 2, 0 );
        ui.scrobbles = new QLabel;
        ui.scrobbles->setObjectName("value");
        grid->addWidget(ui.scrobbles, 2, 1);

        label = new QLabel(tr("Tagged as"));
        label->setObjectName("name");
        label->setProperty("alternate", QVariant(true));
        label->setAlignment( Qt::AlignTop );
        grid->addWidget( label, 3, 0 );
        ui.tags = new QLabel;
        ui.tags->setObjectName("value");
        ui.tags->setProperty("alternate", QVariant(true));
        ui.tags->setWordWrap(true);
        grid->addWidget(ui.tags, 3, 1);

        // bio:
        label = new QLabel(tr("Biography"));
        label->setObjectName("name");
        label->setAlignment( Qt::AlignTop );
        grid->addWidget( label, 4, 0 );
        grid->addWidget(ui.bio = new QTextBrowser, 4, 1);
        ui.bio->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
        ui.bio->setOpenLinks( false );
        grid->setRowStretch( 4, 1 );

        vs->addLayout(grid, 1);
        vs->addStretch();

    }
    connect(ui.bio->document()->documentLayout(), SIGNAL( documentSizeChanged(QSizeF)), SLOT( onBioChanged(QSizeF)));
    connect(ui.bio, SIGNAL(anchorClicked(QUrl)), SLOT(onAnchorClicked(QUrl)));
    vs->setStretchFactor(ui.bio, 1);

    // status bar and scrobble controls
    {
        QStatusBar* status = new QStatusBar( this );
        
        //FIXME: this code is duplicated in the radio too
        //In order to compensate for the sizer grip on the bottom right
        //of the window, an empty QWidget is added as a spacer.
        QSizeGrip* sg = status->findChild<QSizeGrip *>();
        if( sg ) {
            int gripWidth = sg->sizeHint().width();
            QWidget* w = new QWidget( status );
            w->setFixedWidth( gripWidth );
            status->addWidget( w );
        }

        //Seemingly the only way to get a central widget in a QStatusBar
        //is to add an empty widget either side with a stretch value.
        status->addWidget( new QWidget( status), 1 );
        status->addWidget( new ScrobbleControls());
        status->addWidget( new QWidget( status), 1 );
        setStatusBar( status );
    }

    v->setSpacing(0);
    v->setMargin(0);
    vs->setSpacing(0);
    vs->setMargin(0);


    setWindowTitle(tr("Last.fm Audioscrobbler"));
    setUnifiedTitleAndToolBarOnMac( true );
    setMinimumHeight( 80 );
    resize(20, 500);
}

void
MetadataWindow::onAnchorClicked( const QUrl& link )
{
    QDesktopServices::openUrl( link );
}

void
MetadataWindow::onBioChanged( const QSizeF& size )
{
    ui.bio->setMinimumHeight( size.toSize().height() );
}

void
MetadataWindow::onTrackStarted(const Track& t, const Track& previous)
{
    const unsigned short em_dash = 0x2014;
    QString title = QString("%1 ") + QChar(em_dash) + " %2";
    ui.title->setText(title.arg(t.artist()).arg(t.title()));
    ui.album->setText("from " + t.album().title());

    if (t.artist() == previous.artist()) return;

    ui.bio->clear();
    ui.artist_image->clear();
    ui.now_playing_source->onTrackStarted(t, previous);

    m_currentTrack = t;
    
    connect(t.artist().getInfo(), SIGNAL(finished()), SLOT(onArtistGotInfo()));
//    connect(t.album().getInfo(), SIGNAL(finished()), SLOT(onAlbumGotInfo()));
}

void
MetadataWindow::onArtistGotInfo()
{
    XmlQuery lfm = static_cast<QNetworkReply*>(sender())->readAll();

    QString scrobbles = lfm["artist"]["stats"]["playcount"].text();
    QString listeners = lfm["artist"]["stats"]["listeners"].text();
    QString tags;
    foreach(const XmlQuery& e, lfm["artist"]["tags"].children("tag")) {
        if (tags.length()) {
            tags += ", ";
        }
        tags += e["name"].text();
    }

    ui.scrobbles->setText(scrobbles);
    ui.listeners->setText(listeners);
    ui.tags->setText(tags);

    //TODO if empty suggest they edit it
    QString style = "<style>" + ((audioscrobbler::Application*)qApp)->loadedStyleSheet() + styleSheet() + "</style>";
    
    QString bio;
    {
        QStringList bioList = lfm["artist"]["bio"]["content"].text().trimmed().split( "\r" );
        foreach( const QString& p, bioList )
            bio += "<p>" + p + "</p>";
    }

    ui.bio->setHtml( style + bio );

    QTextFrame* root = ui.bio->document()->rootFrame();
    QTextFrameFormat f = root->frameFormat();
    f.setMargin(12);
    root->setFrameFormat(f);

    QUrl url = lfm["artist"]["image size=large"].text();
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

    ui.artist_image->setFixedSize( px.size());
    ui.artist_image->setPixmap(px);
}

void
MetadataWindow::onStopped()
{
    ui.bio->clear();
    ui.artist_image->clear();
    ui.title->clear();
    ui.tags->clear();
    ui.album->clear();
    ui.listeners->clear();
    ui.scrobbles->clear();
    m_currentTrack = Track();
    ui.now_playing_source->onTrackStopped();
}
