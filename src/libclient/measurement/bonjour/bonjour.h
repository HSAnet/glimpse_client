#ifndef BONJOUR_H
#define BONJOUR_H

#include "../measurement.h"
#include <QObject>
#include <QTimer>


class Bonjour : public Measurement
{
public:
    Bonjour();
    ~Bonjour();

    static const int _exploreInterval = 2000;
    static const int _browseInterval = 300;

    // Measurement interface
    Status status() const;
    bool prepare(NetworkManager *networkManager, const MeasurementDefinitionPtr &measurementDefinition);
    bool start();
    bool stop();

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
    void finished();

private:
    BonjourServiceBrowser *_bonjourBrowser;
    BonjourServiceResolver *_bonjourResolver;
    QList<BonjourRecord> _allRecords;
    QMultiMap<QString, QMap<QString, QVariant> > _results;
};

#endif // BONJOUR_H
