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
 
#ifndef SEED_H
#define SEED_H

#include <QListWidgetItem>
#include <QNetworkReply>
#include "lib/lastfm/ws/WsAccessManager.h"
#include "app/moose.h"

class Seed : public QObject
{
    Q_OBJECT
    
public:
	Seed( class SeedListView* parent = 0 );
	Seed( const QString& name, SeedListView* parent = 0 );

    enum Type { 
        Undefined = -1,
        ArtistType = 0, 
        TagType, 
        UserType, 
        PreDefinedType, 
        CustomType
    };
    
    void setupItem()
    {}
    
	static Seed* createFromMimeData( const class PlayableMimeData* data, SeedListView* parent = 0 );
	
	void setPlayableType( const Seed::Type t ){ m_type = t; }
    Seed::Type playableType() const{ return m_type; }
    
    Qt::ItemFlags flags() const{ return Qt::ItemIsDragEnabled | Qt::ItemIsEnabled; }

    QString rql() const{ return m_rql; }
    
    void fetchImage();
    
    void setPixmap( const QPixmap );
    
    void setRQL( const QString& rql ){ m_rql = rql; }
    
    void setIcon( const QIcon& i ){ m_icon = i; emit updated(); }
    const QIcon& icon() const{ return m_icon; }
    
    void setName( const QString& name ){ m_name = name; emit updated(); }
    const QString& name() const{ return m_name; }
    
signals:
    void updated();
    
public slots:
    void iconDataDownloaded();
    
private slots:
    void onArtistSearchFinished( class WsReply* r );
    
private:
    QPixmap cropToSize( const QPixmap, const QSize& ) const;
    QPixmap overlayPixmap( const QPixmap source, const QPixmap overlay, const QPoint offset = QPoint( 0, 0)) const;
    class WsAccessManager* m_networkManager;
    QString m_rql;
    Type m_type;
    QString m_name;
    QIcon m_icon;
    
};

Q_DECLARE_METATYPE( Seed::Type )

#endif //SEED_H
