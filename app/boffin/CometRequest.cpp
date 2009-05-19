#include "CometRequest.h"
#include <QUuid>
#include "json_spirit/json_spirit.h"
#include "jsonGetMember.h"

CometRequest::CometRequest()
:m_qid(QUuid::createUuid().toString().mid(1, 36))
{
}

const QString& 
CometRequest::qid() const
{
    return m_qid;
}

bool 
CometRequest::getQueryId(const QByteArray& data, QString& out)
{
    json_spirit::Value v;
    if (json_spirit::read(std::string(data.constData(), data.size()), v)) {
        std::string qid;
        if (jsonGetMember(v, "qid", qid)) {
            out = QString::fromStdString(qid);
            return true;
        }
    }
    return false;
}

