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
#include <lastfm/Track>
#include <QTextEdit>
#include <QLayout>
#include <QWidget>
class TagBucket;


class TagBuckets : public QWidget
{
    Q_OBJECT
    
    Track m_track;
    int m_current_index;
    
public:
    TagBuckets( const Track& );
    
    struct {
        TagBucket* track;
        TagBucket* artist;
        TagBucket* album;
    } ui;
    
    TagBucket* currentBucket() { return (TagBucket*)layout()->itemAt( m_current_index )->widget(); }

signals:
    void suggestedTagsRequest( QNetworkReply* );
    
private slots:
    void onHeaderClicked();
};


class TagBucket : public QTextEdit
{
    Q_OBJECT

    QStringList m_existingTags;

public:
    TagBucket();
       
    QStringList tags() const;
    QStringList newTags() const;
    QStringList deletedTags() const;
    
public slots:
    void onGotTags( QNetworkReply* );
    
protected:
    virtual void paintEvent( QPaintEvent* );
    virtual void dropEvent( QDropEvent* );
    virtual void dragMoveEvent( QDragMoveEvent* );
    virtual void dragEnterEvent( QDragEnterEvent* );
};
