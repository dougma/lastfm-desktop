#ifndef PLAYLIST_WIDGET_H_
#define PLAYLIST_WIDGET_H_

#include <QTreeView>
#include "PlaylistModel.h"
class PlaylistWidget : public QTreeView
{
Q_OBJECT
public:
    PlaylistWidget( QWidget* p = 0 ): QTreeView( p )
    {
        setAlternatingRowColors( true );
        connect( this, SIGNAL( doubleClicked(QModelIndex)), SLOT( onDoubleClicked(QModelIndex)));
    }

signals:
    void play( const QUrl& );

private slots:
    void onDoubleClicked( const QModelIndex& index )
    {
    	qDebug() << "Play: " << index.data( PlaylistModel::UrlRole ).toUrl().toString();
        emit play( index.data( PlaylistModel::UrlRole ).toUrl());
    }
};

#endif //PLAYLIST_WIDGET_H_
