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
#include "TagBuckets.h"
#include "common/qt/reverse.cpp"
#include "PlayableMimeData.h"
#include <lastfm/Tag>
#include <QtGui>

static int gint = 0;


struct Header : QAbstractButton
{    
    QString m_title, m_classification;
    
    Header( const QString& title, const QString& classification )
    {
        setAcceptDrops( true );
        m_title = title;
        m_classification = classification;
        m_dragTimer.setInterval( 500 );
        m_dragTimer.setSingleShot( true );
        
        connect( &m_dragTimer, SIGNAL( timeout()), SLOT( click()));
    }
    
    QString classification() const { return m_classification; }
    QSize sizeHint() const { return QSize( 100, 26 ); }
    static QFont smallFont() { QFont f; f.setPixelSize( 10 ); return f; }
    
    virtual void paintEvent( QPaintEvent* )
    {
        QPainter p( this );
        
        QLinearGradient g( 0, 0, 0, sizeHint().height() );
        g.setColorAt( 0, 0x3c3939 );
        g.setColorAt( 1, 0x282727 );
        p.fillRect( rect(), g );
        
        int w = p.fontMetrics().width( m_title );
        int maxw = width() - (gint + 10)*2; // two because text is aligned at the center
        
        p.setPen( Qt::white );
        
        if (w >= maxw)
        {
            QString title = p.fontMetrics().elidedText( m_title, Qt::ElideRight, width() - gint - 18 );
            QRect r = rect();
            r.setLeft( gint + 10 );
            p.drawText( r, Qt::AlignVCenter | Qt::AlignLeft, title );
        }
        else
            p.drawText( rect(), Qt::AlignCenter, m_title );
        
        p.setFont( smallFont() );        
        p.setPen( 0x848383 );
        p.drawText( rect().translated( 5, 0 ), Qt::AlignVCenter | Qt::AlignLeft, m_classification );
    }
    
    virtual void dragEnterEvent( QDragEnterEvent* e )
    {
        m_dragTimer.start();
        e->accept();
    }
    
    virtual void dragLeaveEvent( QDragLeaveEvent* )
    {
        m_dragTimer.stop();
    }
    
    QTimer m_dragTimer;
};


TagBuckets::TagBuckets( const Track& t )
{   
    m_track = t;
    
    Header* h1, *h2, *h3;
    
    QVBoxLayout* v = new QVBoxLayout( this );
    v->addWidget( h1 = new Header( t.album(), tr("Album") ) );
    v->addWidget( ui.album = new TagBucket );
    v->addSpacing( 2 );
    v->addWidget( h2 = new Header( t.artist(), tr("Artist") ) );
    v->addWidget( ui.artist = new TagBucket );
    v->addSpacing( 2 );
    v->addWidget( h3 = new Header( t.title() + " (" + t.durationString() + ')', tr("Track") ) );
    v->addWidget( ui.track = new TagBucket );
    v->setSpacing( 0 );
    v->setMargin( 0 );
    
    QFontMetrics metrics( Header::smallFont() );
    foreach (Header* header, QList<Header*>() << h1 << h2 << h3)   
        gint = qMax( metrics.width( header->classification() ), gint );
    
    ui.album->hide();
    ui.artist->hide();
    ui.track->show();
    
    m_current_index = v->indexOf( ui.track );
    
    foreach (QAbstractButton* button, findChildren<QAbstractButton*>())
        connect( button, SIGNAL(clicked()), SLOT(onHeaderClicked()) );
}


void
TagBuckets::onHeaderClicked()
{
    layout()->itemAt( m_current_index )->widget()->hide();
    m_current_index = layout()->indexOf( (QWidget*)sender() ) + 1;
    layout()->itemAt( m_current_index )->widget()->show();
    
    switch (m_current_index / 3)
    {
        case 1: emit suggestedTagsRequest( m_track.artist().getTopTags() ); break;
        case 2: emit suggestedTagsRequest( m_track.getTopTags() ); break;
    };
}




TagBucket::TagBucket()
{    
    setAcceptDrops( true );
}


void
TagBucket::onGotTags( QNetworkReply* r )
{
    setText( Qt::reverse( Tag::list( r ).values() ).join( ", " ) );
}


QStringList
TagBucket::tags() const
{
    //FIXME do properly! ie. get correct disallowed characters and apply the restrictions
    return toPlainText().split( QRegExp( "[ \t]*(,|\n)[ \t]*" ) );
}


QStringList
TagBucket::newTags() const 
{
    QStringList tags = this->tags();
    foreach (QString tag, m_existingTags)
        tags.removeAll( tag );
    return tags;
}


QStringList
TagBucket::deletedTags() const
{
    QStringList deleted_tags;
    QStringList const tags = this->tags();
    foreach (QString tag, m_existingTags)
        if (!tags.contains( tag ))
            deleted_tags += tag;
    return deleted_tags;
}


//////////////////////////
//// Event Handling: /////
//////////////////////////

void 
TagBucket::paintEvent( QPaintEvent* e )
{
    if( toPlainText().isEmpty() )
    {
        QPainter p( viewport() );
        p.setClipRect( e->rect() );
        QString noItemsText = tr( "Drag existing tags from the lists or type to invent your own." );
        p.setBrush( Qt::darkGray );
        p.setPen( Qt::darkGray );
        
        QRect r = viewport()->rect();
        r.adjust( 10, 10, -10, -10 );
        p.drawText( r, Qt::AlignCenter|Qt::TextWordWrap, noItemsText);
    }
    
    QTextEdit::paintEvent( e );
}


void 
TagBucket::dropEvent( QDropEvent* e )
{    
    const PlayableMimeData* playableData;
    if( !( playableData = dynamic_cast< const PlayableMimeData* >( e->mimeData() )))
        return e->ignore();
    
    if( playableData->type() != Seed::TagType )
        return e->ignore();
    
    setText( toPlainText() + 
             (toPlainText().isEmpty() ? "" : ", ") +
             playableData->text() );   
    
    e->accept();
}


void 
TagBucket::dragMoveEvent( QDragMoveEvent* e )
{
    e->accept();
}


void 
TagBucket::dragEnterEvent( QDragEnterEvent* e )
{
    e->accept();
}
