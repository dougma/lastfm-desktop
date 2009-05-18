#ifndef COMET_REQUEST_H
#define COMET_REQUEST_H

#include <QString>
#include <QVariant>
#include <QByteArray>

class CometRequest : public QObject
{
    Q_OBJECT

public:
    CometRequest();
    const QString& qid() const;
    virtual void receiveResult(const QVariantMap& o) = 0;

protected:
    bool getQueryId(const QByteArray& data, QString& out);

    QString m_qid;
};

#endif
