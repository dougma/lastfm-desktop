#ifndef PLAY_LIST_H
#define PLAY_LIST_H

#include <QList>
#include <QString>
#include <QObject>
#include "BoffinRqlRequest.h"

class PlaydarConnection;

typedef QList<BoffinPlayableItem> ItemList;

class Playlist : public QObject
{
    Q_OBJECT

public:
    Playlist(QObject *parent = 0);
    void startRequest(const QString& rql, PlaydarConnection* pc);

private slots:
    void onItem(const BoffinPlayableItem& item);
    void onAfterDelay();

private:
    int m_lastSize;

    ItemList m_playlist;       // shuffled into here
};

#endif
