#include "extupnp.h"

#include "../../log/logger.h"
#include <QMetaEnum>
#include "../../types.h"

LOGGER(ExtUPnP);

#include <miniupnpc/miniupnpc.h>
#include <miniupnpc/upnpcommands.h>
#include <miniupnpc/miniwget.h>

#ifndef FALSE
#define FALSE 0
#define TRUE 1
#endif

ExtUPnP::ExtUPnP(QObject *parent)
: Measurement(parent)
{
}

ExtUPnP::~ExtUPnP()
{
}

Measurement::Status ExtUPnP::status() const
{
    return Unknown;
}

bool ExtUPnP::prepare(NetworkManager *networkManager, const MeasurementDefinitionPtr &measurementDefinition)
{
    Q_UNUSED(networkManager);
    Q_UNUSED(measurementDefinition);
    return true;
}

QStringList GetValsFromNameValueList(struct NameValueParserData *pdata,
                                       const char *Name)
{
    QStringList ret;
    struct NameValue *nv;

    for (nv = pdata->l_head;
         (nv != NULL);
         nv = nv->l_next)
    {
        if (strcmp(nv->name, Name) == 0)
        {
            ret.append(nv->value);
        }
    }

    return ret;
}

bool ExtUPnP::start()
{
    int error = 0;

    /* The following devices are important */
    static const char * const deviceList[] = {
        "urn:schemas-upnp-org:device:MediaServer:1",
        "urn:schemas-upnp-org:device:InternetGatewayDevice:1",
        0
    };
    // TODO test other devices from upnp.org
    /* more or less unimportant: */
//        "urn:schemas-upnp-org:service:ConnectionManager:1",
//        "urn:schemas-upnp-org:service:ContentDirectory:1",
//        "urn:schemas-upnp-org:device:InternetGatewayDevice:2",
//        "urn:schemas-upnp-org:service:WANIPConnection:2",
//        "urn:schemas-upnp-org:service:WANIPConnection:1",
//        "urn:schemas-upnp-org:service:WANPPPConnection:1",
//        "upnp:rootdevice",
//        "ssdp:all",

    UPNPDev *devices = upnpDiscoverDevices(deviceList,
                                           5000, NULL, NULL, FALSE,
                                           FALSE, &error);
    QList<ExtUPnPHash> list = goThroughDeviceList(devices);

    /* Following functions can be called, see miniupnpc.h */
//    UPNPDev *allDevList = upnpDiscoverAll(5000, NULL, NULL, FALSE, FALSE, &error);
//    UPNPDev *IGDlist = ::upnpDiscover(2000, NULL, NULL, FALSE, FALSE, &error);
//    UPNPDev *mediaServerList = ::upnpDiscoverDevice("urn:schemas-upnp-org:device:MediaServer:1",2000, NULL, NULL, FALSE, FALSE, &error);

    emit finished();
    return true; // TODO return false if something went wrong or if there are no results
}

bool ExtUPnP::stop()
{
    return true;
}

QList<ExtUPnP::ExtUPnPHash> ExtUPnP::goThroughDeviceList(UPNPDev *list)
{
    UPNPDev *listBegin = list;
    QList<ExtUPnPHash> myResults;

    while (list)
    {
        UPNPUrls urls;
        IGDdatas data;
        char lanaddr[64];
        ExtUPnPHash resultHash;
        int code = UPNP_GetValidIGD(list, &urls, &data, lanaddr, sizeof(lanaddr));
        if(code > 0)
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
                QStringList modelName = GetValsFromNameValueList(&pdata, "modelName");

                if (!modelName.isEmpty())
                {
                    resultHash.insert(ModelName, modelName.last());
                }

                QStringList manufacturer = GetValsFromNameValueList(&pdata, "manufacturer");

                if (!manufacturer.isEmpty())
                {
                    resultHash.insert(Manufacturer, manufacturer.last());
                }

                QStringList friendlyName = GetValsFromNameValueList(&pdata, "friendlyName");

                if (!friendlyName.isEmpty())
                {
                    resultHash.insert(FriendlyName, friendlyName.last());
                }
                qDebug() << friendlyName;// + modelName + manufacturer;

                ClearNameValueList(&pdata);
            }
        }
        /* These URLs will be needed for accessing and controlling Mediaservers with SOAP */
        QString controlURL = urls.controlURL;
        if(!controlURL.isEmpty())
        {
            resultHash.insert(ControlURL, controlURL);
        }
        QString rootDescURL = urls.rootdescURL;
        if(!rootDescURL.isEmpty())
        {
            resultHash.insert(RootDescURL, rootDescURL);
        }
        FreeUPNPUrls(&urls);
        results.append(resultHash);
        myResults.append(resultHash);
        list = list->pNext;
    }
    freeUPNPDevlist(listBegin);
    return myResults;
}

Result ExtUPnP::result() const
{
     // List for all results
    QVariantList deviceResultList;

    foreach (ExtUPnPHash resultHash, results)
    {
        QHashIterator<ExtUPnP::DataType, QVariant> iter(resultHash);

        // results from one interface
        QVariantMap deviceResult;

        while (iter.hasNext())
        {
            iter.next();
            QString name = enumToString(ExtUPnP, DataType, iter.key());
            name = name.replace(QRegExp("([A-Z])"), "_\\1").toLower();
            name.remove(0, 1);

            deviceResult.insert(name, iter.value());
        }

        deviceResultList.append(deviceResult);
    }

    QVariantMap res;
    res.insert("data", deviceResultList);
    return Result(res);
}
