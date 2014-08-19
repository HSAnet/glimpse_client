#ifndef GETCONFIGRESPONSE_H
#define GETCONFIGRESPONSE_H

#include "response.h"

class CLIENT_API GetConfigResponse : public Response
{
    Q_OBJECT
    Q_PROPERTY(QString supervisorAddress READ supervisorAddress CONSTANT)

public:
    GetConfigResponse(QObject *parent = 0);

    bool fillFromVariant(const QVariantMap &variant);
    QVariant toVariant() const;
    void finished();

    QString supervisorAddress() const;
    TimingPtr supervisorTiming() const;
    QString keepaliveAddress() const;
    TimingPtr keepaliveTiming() const;
    void setConfigAddress(const QString &address);
    QString configAddress() const;
    TimingPtr configTiming() const;
    QString reportAddress() const;
    TimingPtr reportTiming() const;


signals:
    void responseChanged();

protected:
    Channel m_supervisorChannel;
    Channel m_keepaliveChannel;
    Channel m_configChannel;
    Channel m_reportChannel;
};

#endif // GETCONFIGRESPONSE_H
