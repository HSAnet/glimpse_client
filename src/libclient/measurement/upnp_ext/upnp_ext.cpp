#include "upnp_ext.h"

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

bool UPnP::start()
{
    int error = 0;

    /* The following devices are important */
    static const char * const deviceList[] = {
        "urn:schemas-upnp-org:device:MediaServer:1",
        "urn:schemas-upnp-org:device:InternetGatewayDevice:1",

        0
    };

    /* more or less unimportant: */
//        "urn:schemas-upnp-org:service:ConnectionManager:1",
//        "urn:schemas-upnp-org:service:ContentDirectory:1",
//        "upnp:rootdevice",
//        "urn:schemas-upnp-org:device:InternetGatewayDevice:2",
//        "urn:schemas-upnp-org:service:WANIPConnection:2",
//        "urn:schemas-upnp-org:service:WANIPConnection:1",
//        "urn:schemas-upnp-org:service:WANPPPConnection:1",
//        "ssdp:all",

    UPNPDev *devices = upnpDiscoverDevices(deviceList,
                                           5000, NULL, NULL, FALSE,
                                           FALSE, &error);
    QList<UPnPHash> list = goThroughDeviceList(devices);

    /* Following functions can be called, see miniupnpc.h */
//    UPNPDev *allDevList = upnpDiscoverAll(5000, NULL, NULL, FALSE, FALSE, &error);
//    UPNPDev *IGDlist = ::upnpDiscover(2000, NULL, NULL, FALSE, FALSE, &error);
//    UPNPDev *mediaServerList = ::upnpDiscoverDevice("urn:schemas-upnp-org:device:MediaServer:1",2000, NULL, NULL, FALSE, FALSE, &error);

    emit finished();
    return true; // TODO return false if something went wrong or if there are no results
}

bool UPnP::stop()
{
    return true;
}

QList<UPnP::UPnPHash> UPnP::goThroughDeviceList(UPNPDev *list)
{
    UPNPDev *listBegin = list;
    QList<UPnPHash> myResults;

    while (list)
    {
        UPNPUrls urls;
        IGDdatas data;
        char lanaddr[64];
        UPnPHash resultHashOfAll;
        QStringList controlURLs;
        QStringList eventSubURLs;
        int code1 = UPNP_GetValidIGD(list, &urls, &data, lanaddr, sizeof(lanaddr));
        if(code1 > 0)
        {
            resultHashOfAll.insert(LanIpAddress, QLatin1String(lanaddr));

            char externalIP[40];

            if (UPNPCOMMAND_SUCCESS == UPNP_GetExternalIPAddress(urls.controlURL,
                                                                 data.first.servicetype,
                                                                 externalIP))
            {
                resultHashOfAll.insert(ExternalIpAddress, QLatin1String(externalIP));
            }

            char connectionType[64];

            if (UPNPCOMMAND_SUCCESS == UPNP_GetConnectionTypeInfo(urls.controlURL,
                                                                  data.first.servicetype,
                                                                  connectionType))
            {
                resultHashOfAll.insert(ConnectionType, QLatin1String(connectionType));
            }

            quint32 uplink, downlink;

            if (UPNPCOMMAND_SUCCESS == UPNP_GetLinkLayerMaxBitRates(urls.controlURL_CIF,
                                                                    data.CIF.servicetype,
                                                                    &downlink, &uplink))
            {
                resultHashOfAll.insert(LinkLayerMaxDownload, downlink);
                resultHashOfAll.insert(LinkLayerMaxUpload, uplink);
            }

            quint32 bytesSent, bytesReceived, packetsSent, packetsReceived;

            bytesSent = UPNP_GetTotalBytesSent(urls.controlURL_CIF,
                                               data.CIF.servicetype);

            if ((unsigned int)UPNPCOMMAND_HTTP_ERROR != bytesSent)
            {
                resultHashOfAll.insert(TotalBytesSent, bytesSent);
            }

            bytesReceived = UPNP_GetTotalBytesReceived(urls.controlURL_CIF,
                                                       data.CIF.servicetype);

            if ((unsigned int)UPNPCOMMAND_HTTP_ERROR != bytesReceived)
            {
                resultHashOfAll.insert(TotalBytesReceived, bytesReceived);
            }

            packetsSent = UPNP_GetTotalPacketsSent(urls.controlURL_CIF,
                                                   data.CIF.servicetype);

            if ((unsigned int)UPNPCOMMAND_HTTP_ERROR != packetsSent)
            {
                resultHashOfAll.insert(TotalPacketsSent, packetsSent);
            }


            packetsReceived = UPNP_GetTotalPacketsReceived(urls.controlURL_CIF,
                                                           data.CIF.servicetype);

            if ((unsigned int)UPNPCOMMAND_HTTP_ERROR != packetsReceived)
            {
                resultHashOfAll.insert(TotalPacketsReceived, packetsReceived);
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
                resultHashOfAll.insert(Status, status);
                resultHashOfAll.insert(Uptime, uptime);
                resultHashOfAll.insert(LastConnectionError, lastConnectionError);
            }

            quint32 num;

            if (UPNPCOMMAND_SUCCESS == UPNP_GetPortMappingNumberOfEntries(urls.controlURL,
                                                                          data.first.servicetype,
                                                                          &num))
            {
                resultHashOfAll.insert(NumberOfPortMappings, num);
            }

            // TODO GetListOfPortMappings do we need this?

            int firewallEnabled, inboundPinholeAllowed;

            if (UPNPCOMMAND_SUCCESS == UPNP_GetFirewallStatus(urls.controlURL,
                                                              data.first.servicetype,
                                                              &firewallEnabled,
                                                              &inboundPinholeAllowed))
            {
                resultHashOfAll.insert(FirewallEnabled, firewallEnabled);
                resultHashOfAll.insert(InboundPinholeAllowed, inboundPinholeAllowed);
            }

            int bufferSize = 0;
            if (char *buffer = (char *)miniwget(urls.rootdescURL, &bufferSize, 0))
            {
                NameValueParserData pdata;
                ParseNameValue(buffer, bufferSize, &pdata);
                free(buffer);
                buffer = NULL;
                QStringList modelName = GetValuesFromNameValueList(&pdata, "modelName");

                /* These URLs will be needed for accessing and controlling Mediaservers with SOAP */
                controlURLs = GetValuesFromNameValueList(&pdata, "controlURL");
                eventSubURLs = GetValuesFromNameValueList(&pdata, "eventSubURL");

                if (!modelName.isEmpty())
                {
                    resultHashOfAll.insert(ModelName, modelName.last());
                }

                QStringList manufacturer = GetValuesFromNameValueList(&pdata, "manufacturer");

                if (!manufacturer.isEmpty())
                {
                    resultHashOfAll.insert(Manufacturer, manufacturer.last());
                }

                QStringList friendlyName = GetValuesFromNameValueList(&pdata, "friendlyName");

                if (!friendlyName.isEmpty())
                {
                    resultHashOfAll.insert(FriendlyName, friendlyName.last());
                }
                qDebug() << friendlyName;// + modelName + manufacturer;

                ClearNameValueList(&pdata);
            }
        }
        QString myIp = urls.controlURL;
        FreeUPNPUrls(&urls);
        results.append(resultHashOfAll);
        myResults.append(resultHashOfAll);
        list = list->pNext;
    }
    freeUPNPDevlist(listBegin);
    return myResults;
}

Result UPnP::result() const
{
     // List for all results
    QVariantList deviceResultList;

    foreach (UPnPHash resultHash, results)
    {
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
        }

        deviceResultList.append(deviceResult);
    }

    QVariantMap res;
    res.insert("data", deviceResultList);
    return Result(res);
}
