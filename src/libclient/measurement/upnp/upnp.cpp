#include "upnp.h"

#include "../../log/logger.h"
#include <QMetaEnum>
#include "../../types.h"

LOGGER(UPnP)

#include <miniupnpc/miniupnpc.h>
#include <miniupnpc/upnpcommands.h>
#include <miniupnpc/miniwget.h>

#ifndef FALSE
#define FALSE 0
#define TRUE 1
#endif

UPnP::UPnP(QObject *parent)
: Measurement(parent)
{   
}

UPnP::~UPnP()
{
}

Measurement::Status UPnP::status() const
{
    return Unknown;
}

bool UPnP::prepare(NetworkManager *networkManager, const MeasurementDefinitionPtr &measurementDefinition)
{
    Q_UNUSED(networkManager);
    Q_UNUSED(measurementDefinition);
    return true;
}

QStringList GetValuesFromNameValueList(struct NameValueParserData * pdata,
                                       const char * Name)
{
    QStringList ret;
    struct NameValue * nv;

    for (nv = pdata->head.lh_first;
        (nv != NULL);
        nv = nv->entries.le_next)
    {
        if(strcmp(nv->name, Name) == 0)
            ret.append(nv->value);
    }

    return ret;
}

bool UPnP::start()
{
    int error = 0;

    UPNPDev* devlist = ::upnpDiscover(2000, NULL, NULL, FALSE, FALSE, &error);
    if (devlist) {
        UPNPUrls urls;
        IGDdatas data;
        char lanaddr[64];

        int code = UPNP_GetValidIGD(devlist, &urls, &data, lanaddr, sizeof(lanaddr));
        if (code == 1) {
            resultHash.insert(LanIpAddress, QLatin1String(lanaddr));

            char externalIP[40];
            if (UPNPCOMMAND_SUCCESS == UPNP_GetExternalIPAddress(urls.controlURL,
                                                                 data.first.servicetype,
                                                                 externalIP)) {
                resultHash.insert(ExternalIpAddress, QLatin1String(externalIP));
            }

            char connectionType[64];
            if(UPNPCOMMAND_SUCCESS == UPNP_GetConnectionTypeInfo(urls.controlURL,
                                                                 data.first.servicetype,
                                                                 connectionType)) {
                resultHash.insert(ConnectionType, QLatin1String(connectionType));
            }

            quint32 uplink, downlink;
            if (UPNPCOMMAND_SUCCESS == UPNP_GetLinkLayerMaxBitRates(urls.controlURL_CIF,
                                                                    data.CIF.servicetype,
                                                                    &downlink, &uplink)) {
                resultHash.insert(LinkLayerMaxDownload, downlink);
                resultHash.insert(LinkLayerMaxUpload, uplink);
            }

            char status[100];
            unsigned int uptime = 0;
            char lastConnectionError[128];
            if (UPNPCOMMAND_SUCCESS == UPNP_GetStatusInfo(urls.controlURL,
                                                          data.first.servicetype,
                                                          status,
                                                          &uptime,
                                                          lastConnectionError)) {
                resultHash.insert(Status, status);
                resultHash.insert(Uptime, uptime);
                resultHash.insert(LastConnectionError, lastConnectionError);
            }

            int bufferSize = 0;
            if (char* buffer = (char*)miniwget(urls.rootdescURL, &bufferSize, 0)) {
                NameValueParserData pdata;
                ParseNameValue(buffer, bufferSize, &pdata);
                free(buffer); buffer = NULL;

                QStringList modelName = GetValuesFromNameValueList(&pdata, "modelName");
                if ( !modelName.isEmpty() )
                    resultHash.insert(ModelName, modelName.last());

                QStringList manufacturer = GetValuesFromNameValueList(&pdata, "manufacturer");
                if ( !manufacturer.isEmpty() )
                    resultHash.insert(Manufacturer, manufacturer.last());

                QStringList friendlyName = GetValuesFromNameValueList(&pdata, "friendlyName");
                if ( !friendlyName.isEmpty() )
                    resultHash.insert(FriendlyName, friendlyName.last());
            }
        }

        FreeUPNPUrls(&urls);
        freeUPNPDevlist(devlist);

        return true;
    }

    return false;
}

bool UPnP::stop()
{
    return true;
}

ResultPtr UPnP::result() const
{
    // Prepare upnp data
    QVariantMap upnp;
    QHashIterator<UPnP::DataType, QVariant> iter(resultHash);
    while ( iter.hasNext() ) {
        iter.next();

        QString name = enumToString(UPnP, "DataType", iter.key());
        name = name.replace(QRegExp("([A-Z])"), "_\\1").toLower();
        name.remove(0, 1);

        upnp.insert(name, iter.value());
    }

    return ResultPtr(new Result(QDateTime::currentDateTime(), upnp, QVariant()));
}

