#include "upnp.h"

#include "../../log/logger.h"
#include <QMetaEnum>
#include "../../types.h"

LOGGER(UPnP);

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
    setResultHeader(QStringList() << "results");
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

QStringList GetValuesFromNameValueList(struct NameValueParserData *pdata,
                                       const char *Name)
{
    QStringList ret;
    struct NameValue *nv;

    for (nv = pdata->head.lh_first;
         (nv != NULL);
         nv = nv->entries.le_next)
    {
        if (strcmp(nv->name, Name) == 0)
        {
            ret.append(nv->value);
        }
    }

    return ret;
}

bool UPnP::start()
{
    int error = 0;
    int devNumber = 0;

    UPNPDev *devlist = ::upnpDiscover(2000, NULL, NULL, FALSE, FALSE, &error);
    UPNPDev *devlistBegin = devlist;

    // get interface list
    /*foreach(interface, interfaces) {
        newdevlist = ::upnpDiscover(2000, interface, NULL, FALSE, FALSE, &error);

        // go to end of list and append
    }*/

    while (devlist)
    {
        UPNPUrls urls;
        IGDdatas data;
        char lanaddr[64];
        UPnPHash resultHash;

        LOG_DEBUG(QString("Checking device %1: %2").arg(devNumber++).arg(QString::fromLatin1(devlist->descURL)));

        int code = UPNP_GetValidIGD(devlist, &urls, &data, lanaddr, sizeof(lanaddr));

        if (code > 0)   // TODO maybe distinguish between the return codes (1,2,3) to add information what happend to the result
        {
            resultHash.insert(LanIpAddress, QLatin1String(lanaddr));

            char externalIP[40];

            if (UPNPCOMMAND_SUCCESS == UPNP_GetExternalIPAddress(urls.controlURL,
                                                                 data.first.servicetype,
                                                                 externalIP))
            {
                resultHash.insert(ExternalIpAddress, QLatin1String(externalIP));
            }

            char connectionType[64];

            if (UPNPCOMMAND_SUCCESS == UPNP_GetConnectionTypeInfo(urls.controlURL,
                                                                  data.first.servicetype,
                                                                  connectionType))
            {
                resultHash.insert(ConnectionType, QLatin1String(connectionType));
            }

            quint32 uplink, downlink;

            if (UPNPCOMMAND_SUCCESS == UPNP_GetLinkLayerMaxBitRates(urls.controlURL_CIF,
                                                                    data.CIF.servicetype,
                                                                    &downlink, &uplink))
            {
                resultHash.insert(LinkLayerMaxDownload, downlink);
                resultHash.insert(LinkLayerMaxUpload, uplink);
            }

            quint32 bytesSent, bytesReceived, packetsSent, packetsReceived;

            bytesSent = UPNP_GetTotalBytesSent(urls.controlURL_CIF,
                                               data.CIF.servicetype);

            if ((unsigned int)UPNPCOMMAND_HTTP_ERROR != bytesSent)
            {
                resultHash.insert(TotalBytesSent, bytesSent);
            }

            bytesReceived = UPNP_GetTotalBytesReceived(urls.controlURL_CIF,
                                                       data.CIF.servicetype);

            if ((unsigned int)UPNPCOMMAND_HTTP_ERROR != bytesReceived)
            {
                resultHash.insert(TotalBytesReceived, bytesReceived);
            }

            packetsSent = UPNP_GetTotalPacketsSent(urls.controlURL_CIF,
                                                   data.CIF.servicetype);

            if ((unsigned int)UPNPCOMMAND_HTTP_ERROR != packetsSent)
            {
                resultHash.insert(TotalPacketsSent, packetsSent);
            }


            packetsReceived = UPNP_GetTotalPacketsReceived(urls.controlURL_CIF,
                                                           data.CIF.servicetype);

            if ((unsigned int)UPNPCOMMAND_HTTP_ERROR != packetsReceived)
            {
                resultHash.insert(TotalPacketsReceived, packetsReceived);
            }


            char status[100];
            unsigned int uptime = 0;
            char lastConnectionError[128];

            if (UPNPCOMMAND_SUCCESS == UPNP_GetStatusInfo(urls.controlURL,
                                                          data.first.servicetype,
                                                          status,
                                                          &uptime,
                                                          lastConnectionError))
            {
                resultHash.insert(Status, status);
                resultHash.insert(Uptime, uptime);
                resultHash.insert(LastConnectionError, lastConnectionError);
            }

            quint32 num;

            if (UPNPCOMMAND_SUCCESS == UPNP_GetPortMappingNumberOfEntries(urls.controlURL,
                                                                          data.first.servicetype,
                                                                          &num))
            {
                resultHash.insert(NumberOfPortMappings, num);
            }

            // TODO GetListOfPortMappings do we need this?

            int firewallEnabled, inboundPinholeAllowed;

            if (UPNPCOMMAND_SUCCESS == UPNP_GetFirewallStatus(urls.controlURL,
                                                              data.first.servicetype,
                                                              &firewallEnabled,
                                                              &inboundPinholeAllowed))
            {
                resultHash.insert(FirewallEnabled, firewallEnabled);
                resultHash.insert(InboundPinholeAllowed, inboundPinholeAllowed);
            }

            int bufferSize = 0;

            if (char *buffer = (char *)miniwget(urls.rootdescURL, &bufferSize, 0))
            {
                NameValueParserData pdata;
                ParseNameValue(buffer, bufferSize, &pdata);
                free(buffer);
                buffer = NULL;

                QStringList modelName = GetValuesFromNameValueList(&pdata, "modelName");

                if (!modelName.isEmpty())
                {
                    resultHash.insert(ModelName, modelName.last());
                }

                QStringList manufacturer = GetValuesFromNameValueList(&pdata, "manufacturer");

                if (!manufacturer.isEmpty())
                {
                    resultHash.insert(Manufacturer, manufacturer.last());
                }

                QStringList friendlyName = GetValuesFromNameValueList(&pdata, "friendlyName");

                if (!friendlyName.isEmpty())
                {
                    resultHash.insert(FriendlyName, friendlyName.last());
                }

                ClearNameValueList(&pdata);
            }
        }

        FreeUPNPUrls(&urls);

        results.append(resultHash);
        devlist = devlist->pNext;
    }

    freeUPNPDevlist(devlistBegin);

    emit finished();
    return true; // TODO return false if something went wrong or if there are no results
}

bool UPnP::stop()
{
    return true;
}

Result UPnP::result() const
{
    // List for all results
    QVariantList deviceResultList;

    foreach (UPnPHash resultHash, results)
    {
        LOG_DEBUG("UPnP result:");
        QHashIterator<UPnP::DataType, QVariant> iter(resultHash);

        // results from one interface
        QVariantMap deviceResult;

        while (iter.hasNext())
        {
            iter.next();

            QString name = enumToString(UPnP, DataType, iter.key());
            name = name.replace(QRegExp("([A-Z])"), "_\\1").toLower();
            name.remove(0, 1);

            deviceResult.insert(name, iter.value());

            LOG_DEBUG(QString("%1: %2").arg(name).arg(iter.value().toString()));
        }

        deviceResultList.append(deviceResult);
    }

    // the results need to be in a list, because multiple dict-objects are possible
    return Result(QVariantList()<<QVariant(deviceResultList));
}
