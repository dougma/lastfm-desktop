#include "LocalRqlPlugin.h"
#include "TagUpdater.h"
#include "LocalCollection.h"
#include "SimilarArtists.h"

#include "rqlParser/parser.h"
#include "ResultSet.h"

using namespace std;
using namespace fm::last::query_parser;

struct ResOp
{
   bool isRoot;
   int type;
   double weight;
   string name;
};


ResOp root2op( const querynode_data& node )
{
   ResOp op;
   op.isRoot = true;
   op.name = node.name;
   op.type = node.type;
   op.weight = node.weight;

   return op;
}


ResOp leaf2op( const querynode_data& node )
{
   ResOp op;
   op.isRoot = false;

   if ( node.ID < 0 )
      op.name = node.name;
   else
   {
      ostringstream oss;
      oss << '<' << node.ID << '>';
      op.name = oss.str();
   }
   op.type = node.type;
   op.weight = node.weight;

   return op;
}


//struct Entry
//{
//    uint m_fileId;
//
//    bool operator==(const Entry& that) const
//    {
//        return this->m_fileId == that.m_fileId;
//    }
//};
//
//uint qHash(const Entry& e)
//{
//    return e.m_fileId;
//}


struct UnsupportedResultSet : public ResultSet
{
    UnsupportedResultSet()
        : ResultSet(true)
    {
    }
};



class RqlOpProcessor
{
    std::vector<ResOp>::iterator m_it, m_end;
    LocalCollection& m_collection;
    SimilarArtists& m_similarArtists;

    void next()
    {
        if (++m_it == m_end) {
            throw "unterminated query. how could the parser do this to us??"; // it's an outrage
        }
    }

public:
    RqlOpProcessor(std::vector<ResOp> &ops, LocalCollection& collection, SimilarArtists& similarArtists)
        : m_collection(collection)
        , m_similarArtists(similarArtists)
        , m_it(ops.begin())
        , m_end(ops.end())
    {
    }

    ResultSet process()
    {
        if (m_it->isRoot) {
            // note the operator type, then move on and get the operands
            int op = m_it->type;
            ResultSet a( (next(), process()) );
            ResultSet b( (next(), process()) );
            switch (op) {
                case OT_AND:
                    return a.and(b);

                case OT_OR:
                    return a.or(b);

                case OT_AND_NOT:
                    return a.and_not(b);
            }
            throw "unknown operation";
        }

        // it's a leaf node, or a 'source' if you like
        switch (m_it->type) {
            case RS_LIBRARY:
            case RS_LOVED:
            case RS_NEIGHBORS:
            case RS_PLAYLIST:
            case RS_RECOMMENDED:
            case RS_GROUP:
            case RS_EVENT:
                return unsupported();

            case RS_SIMILAR_ARTISTS:
                return similarArtist();

            case RS_GLOBAL_TAG:
                return globalTag();

            case RS_USER_TAG:
                return userTag();

            case RS_ARTIST:
                return artist();
        }

        throw "unknown field";
    }

    ResultSet unsupported()
    {
        return UnsupportedResultSet();
    }

    ResultSet globalTag()
    {
        return m_collection.filesWithTag(m_it->name.data());
    }

    ResultSet userTag()
    {
        return m_collection.filesWithTag(m_it->name.data());
    }

    ResultSet artist()
    {
        return m_collection.filesByArtist(m_it->name.data());
    }

    ResultSet similarArtist()
    {
        return m_similarArtists.filesBySimilarArtist(m_collection, m_it->name.data());
    }
};


RqlQuery::RqlQuery()
:m_collection(0)
{
}

RqlQuery::~RqlQuery()
{
    delete m_collection;
}

QSet<uint>
RqlQuery::doQuery(const char *rql)
{
    if (!m_collection)
        m_collection = LocalCollection::create("RqlQuery");

    parser p;
    if (p.parse(string(rql))) {
        std::vector<ResOp> ops;
        p.getOperations<ResOp>(ops, &root2op, &leaf2op);
        return RqlOpProcessor(ops, *m_collection, m_similarArtists).process();
    }

    return QSet<uint>();
}



LocalRqlPlugin::LocalRqlPlugin()
: m_tagUpdater(0)
, m_query(0)
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
    if (!m_query)
        m_query = new RqlQuery();
    
    QSet<uint> results = m_query->doQuery(rql);

    int i = results.size();

    return 0;
}

void 
LocalRqlPlugin::finished(ILocalRqlPull* radio)
{
    delete m_tagUpdater;
    delete m_query;
}
