#ifndef BONJOUR_H
#define BONJOUR_H

#include "../measurement.h"
#include "bonjourrecord.h"
#include "bonjourservicebrowser.h"
#include "bonjourserviceresolver.h"

#include <QObject>
#include <QTimer>
#include <QHostInfo>


class Bonjour : public Measurement
{
    Q_OBJECT

public:
    Bonjour(QObject *parent = 0);
    ~Bonjour();

    // Measurement interface
    Status status() const;
    bool prepare(NetworkManager *networkManager, const MeasurementDefinitionPtr &measurementDefinition);
    bool start();
    bool stop();

    static const int _exploreInterval = 2000;
    static const int _browseInterval = 300;

private slots:
    void updateRecords(const QList<BonjourRecord> &list);
    void saveHostInformation(const QHostInfo &hostInfo);
    void checkResults();
    void startRecordResolve();
    void getRecord();
    void prepareForNextRecord();
    void displayResults();

signals:
    void hostInfoSaved();
    void done();

private:
    BonjourServiceBrowser *_bonjourBrowser;
    BonjourServiceResolver *_bonjourResolver;
    QList<BonjourRecord> _allRecords;
    QMultiMap<QString, QMap<QString, QVariant> > _results;
};

#endif // BONJOUR_H
