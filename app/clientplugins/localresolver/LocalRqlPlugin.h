#ifndef LOCAL_RQL_H
#define LOCAL_RQL_H

#include "../ILocalRql.h"


class LocalRqlPlugin : public ILocalRqlPlugin
{
    class TagUpdater* m_tagUpdater;

public:
    LocalRqlPlugin();
    ~LocalRqlPlugin();

    // ILocalRqlPlugin:
    void init();
	ILocalRqlPull* play(const char *rql);
    void finished(ILocalRqlPull* radio);
};

#endif