#include "LocalRqlPlugin.h"
#include "TagUpdater.h"


LocalRqlPlugin::LocalRqlPlugin()
: m_tagUpdater(0)
{
}

void 
LocalRqlPlugin::init()
{
    m_tagUpdater = new TagUpdater();
    m_tagUpdater->start();
}

ILocalRqlPull* 
LocalRqlPlugin::play(const char *rql)
{
    //todo
    return 0;
}

void 
LocalRqlPlugin::finished(ILocalRqlPull* radio)
{
    //todo
}
