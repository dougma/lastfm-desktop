#ifndef WS_REQUEST_PARAMETERS_H
#define WS_REQUEST_PARAMETERS_H

#include <QString>
#include <QList>
#include <QPair>
#include <QMap>
#include <QObject>
#include "lib/DllExportMacro.h"

class UNICORN_DLLEXPORT WsRequestParameters: public QObject
{
    Q_OBJECT
public:
    WsRequestParameters( QObject* parent = 0 );
    ~WsRequestParameters( void );
    
    WsRequestParameters& add( const QString& key, const QString& value );
    operator const QList< QPair< QString, QString > >();

private:
    QMap< QString, QString > m_paramList;
    QString methodSignature();
};

#endif //WS_REQUEST_PARAMETERS_H