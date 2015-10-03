#include "bonjour.h"

Bonjour::Bonjour(QObject *parent)
    : Measurement(parent)
{

}

Bonjour::~Bonjour()
{

}

Measurement::Status Bonjour::status() const
{
    return Unknown;
}

bool Bonjour::prepare(NetworkManager *networkManager, const MeasurementDefinitionPtr &measurementDefinition)
{
    Q_UNUSED(networkManager);

//    definition = measurementDefinition.dynamicCast<UPnPDefinition>();

//    if (definition.isNull())
//    {
//        //setErrorString("received NULL definition");
//        //return false;
//        return true;
//    }
    return true;
}

bool Bonjour::start()
{
    _bonjourBrowser = new BonjourServiceBrowser(this);
    _bonjourResolver = new BonjourServiceResolver(this);
    _bonjourBrowser->setInterval(_browseInterval);
    connect(_bonjourBrowser,  SIGNAL(currentBonjourRecordsChanged(const QList<BonjourRecord> &)), this, SLOT(updateRecords(const QList<BonjourRecord> &)));
    connect(_bonjourBrowser,  SIGNAL(error(DNSServiceErrorType)),                 _bonjourBrowser,      SLOT(handleError(DNSServiceErrorType)));
    connect(_bonjourResolver, SIGNAL(error(DNSServiceErrorType)),                 _bonjourResolver,     SLOT(handleError(DNSServiceErrorType)));
    connect(_bonjourBrowser,  SIGNAL(done()),                                 this,                 SLOT(startRecordResolve()));
    connect(_bonjourResolver, SIGNAL(bonjourRecordResolved(const QHostInfo &)),   this,                 SLOT(saveHostInformation(QHostInfo)));
    connect(_bonjourResolver, SIGNAL(cleanedUp()),                                this,                 SLOT(prepareForNextRecord()));
    connect(this,             SIGNAL(done()),                                 this,                 SLOT(displayResults()));

    QTimer::singleShot(_exploreInterval, this, SLOT(checkResults()));
    _bonjourBrowser->browseForServiceType(QLatin1String("_services._dns-sd._udp"));
    return 0;
}

bool Bonjour::stop()
{
    return true;
}

void Bonjour::updateRecords(const QList<BonjourRecord> &list)
{
    //TODO not needed anymore
}

void Bonjour::saveHostInformation(const QHostInfo &hostInfo)
{
    BonjourRecord r = _allRecords.first();
    QMap<QString, QVariant> results;
    results.insert("registeredType", r.registeredType);
    results.insert("replyDomain", r.replyDomain);
    results.insert("serviceName", r.serviceName);
    results.insert("hostname", hostInfo.hostName());
    results.insert("lookupId", hostInfo.lookupId());

    const QList<QHostAddress> &addresses = hostInfo.addresses();
    foreach (QHostAddress addr, addresses) {
        results.insertMulti("ip", addr.toString());
    }
    _results.insertMulti(r.registeredType, results);
    emit hostInfoSaved();
}

void Bonjour::checkResults()
{
    if(!_bonjourBrowser->_bonjourRecords.isEmpty())
    {
        _bonjourBrowser->cleanUp();
        _bonjourBrowser->browseForFoundServiceTypes();
    }else{
        qDebug() << "no devices found";
        //TODO emit finished(error); or similar
    }
}

void Bonjour::startRecordResolve()
{
    _allRecords = _bonjourBrowser->_bonjourRecords;
    foreach(BonjourRecord b, _allRecords)
    {
        qDebug() << b.registeredType << b.replyDomain << b.serviceName;
    }

    getRecord();
}

void Bonjour::getRecord()
{
    if(!_allRecords.isEmpty())
    {
        //NOTE this parameter would make it faster: maybe put it in measurement definition
        //TODO maybe get parameter for not searching for iphones->takes long, dns_sd maybe, never brings outcomes
//        if(_allRecords.first().registeredType == "_apple-mobdev2._tcp.") /*||
//           _allRecords.first().registeredType == "_dns-sd._udp.")
//        {
//            prepareForNextRecord();
//            return;
//        }
        _bonjourResolver->resolveBonjourRecord(_allRecords.first());

    }else{
        qDebug() << "Done";
        emit done();
    }
}

void Bonjour::prepareForNextRecord()
{
    _allRecords.removeFirst();
    getRecord();
}

void Bonjour::displayResults()
{
    //TODO this shall be moved to result()
    QStringList keys = _results.keys();
    keys = keys.toSet().toList();
    foreach(QString key, keys)
    {
        QList<QMap<QString, QVariant> > membersOfOneClass = _results.values(key);
        qDebug() << key << ":";
        QMap<QString, QVariant> m;
        foreach (m, membersOfOneClass) {
            qDebug() << "\tregisteredType:" << m.value("registeredType").toString();
            qDebug() << "\treplyDomain:"    << m.value("replyDomain").toString();
            qDebug() << "\tserviceName:"    << m.value("serviceName").toString();
            qDebug() << "\tIP:"             << m.value("ip").toString();
            qDebug() << "\thostname:"       << m.value("hostname").toString();
            qDebug() << "\tlookupId:"       << m.value("lookupId").toString();
            qDebug() << "";
        }
    }
    qDebug() << "Found a total of " << _results.size() << "elements in " << keys.length() << "different categories.";
    //QCoreApplication::exit(0);
}

