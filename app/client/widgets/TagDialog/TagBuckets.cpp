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
#include "lib/lastfm/types/Track.h"
#include "PlayableMimeData.h"
#include "SeedTypes.h"
#include <QtGui>


struct Header : QAbstractButton
{
    QString m_title, m_classification;
    
    Header( const QString& title, const QString& classification )
    {
        m_title = title;
        m_classification = classification;
    }
    
    QSize sizeHint() const { return QSize( 100, 26 ); }
    
    virtual void paintEvent( QPaintEvent* )
    {
        QPainter p( this );
        
        QLinearGradient g( 0, 0, 0, sizeHint().height() );
        g.setColorAt( 0, 0x3c3939 );
        g.setColorAt( 1, 0x282727 );
        p.fillRect( rect(), g );
        
        p.setPen( Qt::white );
        p.drawText( rect(), Qt::AlignCenter, m_title );
        
        QFont f = p.font();
        f.setPixelSize( 10 );
        p.setFont( f );
        
        p.setPen( 0x848383 );
        p.drawText( rect().translated( 5, 0 ), Qt::AlignVCenter | Qt::AlignLeft, m_classification );
    }
};


TagBuckets::TagBuckets( const Track& t )
{   
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
