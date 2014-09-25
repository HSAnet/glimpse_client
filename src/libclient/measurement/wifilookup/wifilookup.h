#ifndef WIFI_LOOKUP_H
#define WIFI_LOOKUP_H

#include <QtGlobal>
#include <QVector>
#ifdef Q_OS_ANDROID
#include <QAndroidJniObject>
#endif

#include "../measurement.h"
#include "wifilookup_definition.h"

class WifiLookup: public Measurement
{
    Q_OBJECT

public:
    explicit WifiLookup(QObject *parent = 0);
    ~WifiLookup();

    // Measurement interface
    Status status() const;
    bool prepare(NetworkManager *networkManager,
                 const MeasurementDefinitionPtr &measurementDefinition);
    bool start();
    bool stop();
    Result result() const;

protected:
    class Private;
    Private *d;

private:
    void setStatus(Status status);

    WifiLookupDefinitionPtr definition;
    Status currentStatus;

signals:
    void statusChanged(Status status);
};
#endif // WIFI_LOOKUP_H
