#include "WsReply.h"

QDomDocument WsReply::domDocument()
{
    QDomDocument doc;
    doc.setContent( readAll() );
    return doc;
}