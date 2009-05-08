#ifndef PLAYDAR_API_H
#define PLAYDAR_API_H

#include "TPlaydarApi.hpp"
#include <QUrl>
#include <QString>

typedef QList< QPair<QString,QString> > ParamList;

class PlaydarApiQtPolicy
{
public:
    static void paramsAdd(ParamList& params, const QString& name, const QString& value)
    {
        params.append(QPair< QString, QString>(name, value));
    }

    static QUrl createUrl(const QString& base, const QString& path, const ParamList& params)
    {
        QUrl url(base + path);
        url.setQueryItems(params);
        return url;
    }
};


typedef TPlaydarApi<QString, QUrl, ParamList, PlaydarApiQtPolicy> PlaydarApi;

#endif

