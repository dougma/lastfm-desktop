#include "jsonGetMember.hpp"

bool jsonGetMember(const QVariantMap& o, const char* key, QString& out)
{
    QVariantMap::const_iterator it = o.find(key);
    if (it != o.end() && it->type() == QVariant::String) {
        out = it->toString();
        return true;
    }
    return false;
}

bool jsonGetMember(const QVariantMap& o, const char* key, int& out)
{
    QVariantMap::const_iterator it = o.find(key);
    if (it != o.end() && it->type() == QVariant::LongLong) {
        out = it->toLongLong();
        return true;
    }
    return false;
}

bool jsonGetMember(const QVariantMap& o, const char* key, double& out)
{
    QVariantMap::const_iterator it = o.find(key);
    if (it != o.end() && it->type() == QVariant::Double) {
        out = it->toDouble();
        return true;
    }
    return false;
}
