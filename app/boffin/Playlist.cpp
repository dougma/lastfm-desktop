#include <QTimer>
#include "Playlist.h"
#include "PlaydarConnection.h"
#include "sample/SampleFromDistribution.h"


Playlist::Playlist(QObject *parent /* = 0 */)
: QObject(parent)
{
}

void 
Playlist::startRequest(const QString& rql, PlaydarConnection* pc)
{
    BoffinRqlRequest* req = pc->boffinRql(rql);
    if (req) {
        connect(req, SIGNAL(playableItem(BoffinPlayableItem)), SLOT(onItem(BoffinPlayableItem)));
    }
}

void
Playlist::onItem(const BoffinPlayableItem& item)
{
}

void
Playlist::onAfterDelay()
{
}


