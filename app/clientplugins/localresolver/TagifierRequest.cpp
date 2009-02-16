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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include <QDebug> //first or function stamping doesn't happen
#include "TagifierRequest.h"
#include "LocalCollection.h"
#include "AutoTransaction.h"
#include <QNetworkReply>
#include <QNetworkRequest>

// Batcher used only in this file
class Batcher
{
    // these are from TagifierRequest:
    LocalCollection* m_collection;
    QVariantList& m_requestedFileIds;

    // build up three parallel arrays to feed into updateTrackTags in batches 
    // of up to 5000. we batch to reduce memory usage and to give the db time 
    // to service others
    QVariantList m_fileIds;
    QVariantList m_weights;
    QStringList m_tags;

    // build up a map of tag names to ids:
    QMap<QString, int> m_tagmap;

    // when the first batch is ready, we delete track tags
    bool m_firstBatch;

    void doBatch();

public:
    Batcher(LocalCollection* collection, QVariantList& requestedFileIds);
    ~Batcher();
    void process(int fileId, QString tag, float weight);
};


Batcher::Batcher(LocalCollection* collection, QVariantList& requestedFileIds)
:m_collection(collection)
,m_requestedFileIds(requestedFileIds)
,m_firstBatch(true)
{
}

Batcher::~Batcher()
{
    if (m_tags.size())
        doBatch();        // submit partial remaining batch

    m_collection->setFileTagTime(m_requestedFileIds);
}

void 
Batcher::doBatch()
{
    if (m_firstBatch) {
        // things seem to be going well enough to delete track tags
        m_collection->deleteTrackTags(m_requestedFileIds);
        m_firstBatch = false;
    }
    {
        QMutexLocker locker( m_collection->getMutex() );
        AutoTransaction<LocalCollection> trans(*m_collection);
        m_collection->updateTrackTags(
            m_fileIds, 
            m_collection->resolveTags(m_tags, m_tagmap), 
            m_weights);
        trans.commit();
    }

    m_fileIds.clear();
    m_tags.clear();
    m_weights.clear();
}

void Batcher::process(int fileId, QString tag, float weight)
{
    m_fileIds << fileId;
    m_tags << tag;
    m_weights << weight;
    if (m_tags.size() % 5000 == 0)
        doBatch();
}



///////////////////////


TagifierRequest::TagifierRequest(LocalCollection* collection, QString url)
    :m_reply(0)
    ,m_url(url)
    ,m_collection(collection)
{
    Q_ASSERT(collection);
}

// a false return means there will be no 
// callback because there is no work to do!
bool
TagifierRequest::makeRequest(int maxTagAgeDays)
{
    Q_ASSERT(0 == m_reply);
    if (m_reply) return false;

    {
        QList<LocalCollection::FilesToTagResult> files = m_collection->getFilesToTag(maxTagAgeDays);
        foreach(const LocalCollection::FilesToTagResult& f, files) {
            QString line;
            line += QString::number(f.fileId);
            line += '\t';
            line += f.artist;
            line += '\t';
            line += f.album;
            line += '\t';
            line += f.title;
            line += '\n';
            m_body.append(line.toUtf8());
            m_requestedFileIds << f.fileId;
        }
    }

    m_requestIdCount = m_requestedFileIds.size();
    m_responseIdCount = m_responseTagCount = 0;
    if (m_requestIdCount) {
        m_reply = m_wam.post(QNetworkRequest(m_url), m_body);
        connect(m_reply, SIGNAL(finished()), this, SLOT(onFinished()));
        return true;
    }

    return false;
}


void
TagifierRequest::onFinished()
{
    if (m_reply->error() == QNetworkReply::NoError) {
        handleResponse();
    }

    m_reply->deleteLater();
    m_reply = 0;
    m_body.clear();
    m_requestedFileIds.clear();

    emit finished(m_requestIdCount, m_responseIdCount, m_responseTagCount);
}




void
TagifierRequest::handleResponse()
{
    // response is in the form:
    //     one line per id, 
    //     multiple tag/weight pairs per line,
    //     all terms \t separated
    //     line terminated with \n

    // resp needs to live until all processing is done, because 
    // of the use of fromRawData below.
    QString resp = QString::fromUtf8(m_reply->readAll());
    {
        Batcher batcher(m_collection, m_requestedFileIds);

        // parsing:
        QString::const_iterator p = resp.begin();
        QString::const_iterator word = p;
        while (*p != 0) {
            word = p;
            while (*p != 0 && *p != '\t') p++;
            if (*p == 0) break;
            bool idOk = false;
            unsigned id = QString::fromRawData(word, p-word).toInt(&idOk);
            while (*p != 0 && *p != '\n') {
                word = ++p;
                while (*p != 0 && *p != '\t') p++;
                if (*p == 0) break;
                QString tag = QString::fromRawData(word, p-word);

                word = ++p;
                while (*p != 0 && *p != '\t' && *p != '\n') p++;
                bool weightOk;
                float weight = QString::fromRawData(word, p-word).toFloat(&weightOk);
                if (weightOk && idOk) {
                    batcher.process(id, tag, weight);
                    m_responseTagCount++;
                }
                if (*p == 0) break;
            }
            if (idOk) {
                m_responseIdCount++;
            }

            if (*p == 0) break;
            p++;
        }
    }
}


