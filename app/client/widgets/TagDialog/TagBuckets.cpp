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

#include "TagBuckets.h"
#include "lib/lastfm/types/Tag.h"
#include "PlayableMimeData.h"
#include "SeedTypes.h"
#include <QtGui>

static int gint = 0;


struct Header : QAbstractButton
{    
    QString m_title, m_classification;
    
    Header( const QString& title, const QString& classification )
    {
        m_title = title;
        m_classification = classification;
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


void
TagBucket::onGotTags( WsReply* r )
{
    setText( QStringList(Tag::list( r )).join( ", " ) );
}


QStringList
TagBucket::newTags() const 
{
    //FIXME do properly!
    QStringList tags = toPlainText().split( QRegExp( "[ \t]*(,|\n)[ \t]*" ) );
                                    
    foreach (QString tag, m_existingTags)
        tags.removeAll( tag );
    
    return tags;
}


TagBucket::TagBucket()
{    
    setAcceptDrops( true );
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
        p.drawText( viewport()->rect(), Qt::AlignCenter|Qt::TextWordWrap, noItemsText);
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
