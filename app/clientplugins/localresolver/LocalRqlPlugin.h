#ifndef LOCAL_RQL_H
#define LOCAL_RQL_H

#include "../ILocalRql.h"
#include <QSet>
#include "SimilarArtists.h"

class RqlQuery
{
    class LocalCollection *m_collection;
    SimilarArtists m_similarArtists;

public:
    RqlQuery();
    ~RqlQuery();

    QSet<uint> doQuery(const char *rql);
};


class LocalRqlPlugin : public ILocalRqlPlugin
{
    class TagUpdater* m_tagUpdater;
    class RqlQuery* m_query;

public:
    LocalRqlPlugin();
    ~LocalRqlPlugin();

    // ILocalRqlPlugin:
    void init();
	ILocalRqlPull* play(const char *rql);
    void finished(ILocalRqlPull* radio);
};


#endif