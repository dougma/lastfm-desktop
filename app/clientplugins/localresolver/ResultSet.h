#ifndef RESULT_SET_H
#define RESULT_SET_H

#include <QSet>

class ResultSet : public QSet<uint>
{
    // marks a special kind of result set which 
    // has come from an unsupported rql service name.
    // it behaves differently depending on the operation
    // so as not to ruin the whole query.  :)
    bool m_unsupported; 

protected:
    ResultSet(bool unsupported)
        : m_unsupported(unsupported)
    {
    }

public:
    ResultSet()
        :m_unsupported(false)
    {
    }

    ResultSet(const QSet<uint>& set)
        :QSet<uint>(set)
    {
    }

    ResultSet and(const ResultSet &other)
    {
        intersect(other);
        return *this;
    }

    ResultSet or(const ResultSet &other)
    {
        unite(other);
        return *this;
    }

    ResultSet and_not(const ResultSet &other)
    {
        subtract(other);
        return *this;
    }

};

#endif