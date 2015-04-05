#include "upnp.h"

//#include "../upnpSniffer/UPnPHandler.h"

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

    definition = measurementDefinition.dynamicCast<UPnPDefinition>();

    if (definition.isNull())
    {
        setErrorString("received NULL definition");
        return false;
    }
    m_mediaServerSearch = definition->mediaServerSearch;
    if(m_mediaServerSearch)
    {
        m_handler = new UPnPHandler();
    }
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
    QList<UPnPHash> mediaServerList;
    if(m_mediaServerSearch)
    {
        /* The following devices are important */
        static const char * const deviceList[] = {
            "urn:schemas-upnp-org:device:MediaServer:1",
//            "urn:schemas-upnp-org:service:ContentDirectory:1",
//            "urn:schemas-upnp-org:device:InternetGatewayDevice:1",
//            "ssdp:all"
            0
        };
        // TODO test other devices from upnp.org
        /* more or less unimportant: */
    //        "urn:schemas-upnp-org:service:ConnectionManager:1",
    //        "urn:schemas-upnp-org:device:InternetGatewayDevice:2",
    //        "urn:schemas-upnp-org:service:WANIPConnection:2",
    //        "urn:schemas-upnp-org:service:WANIPConnection:1",
    //        "urn:schemas-upnp-org:service:WANPPPConnection:1",
    //        "upnp:rootdevice",
        UPNPDev *devices = upnpDiscoverDevices(deviceList,
                                               5000, NULL, NULL, FALSE,
                                               FALSE, &error);
        mediaServerList = goThroughDeviceList(devices);
        QUrl url;
        QString descriptionUrl, eventSubUrl, controlUrl, serviceType;
        for(int i = 0; i < 1; i++)
        {
            url = mediaServerList[i].value(URL).toUrl();
            descriptionUrl = mediaServerList[i].value(RootDescURL).toString();
            eventSubUrl = mediaServerList[i].value(EventSubUrl).toString();
            controlUrl = mediaServerList[i].value(ControlURL).toString();
            serviceType = mediaServerList[i].value(ServiceType).toString();
            //TODO own Url handling
            m_handler->init(url, url, descriptionUrl, eventSubUrl, controlUrl, serviceType);
            //TODO wait for execution e.g. new slot here or something else
        }

    }else
    {
        /* This is the old measurement */
        UPNPDev *devlist = ::upnpDiscover(2000, NULL, NULL, FALSE, FALSE, &error);
        QList<UPnPHash> list = goThroughDeviceList(devlist);
    }
    emit finished();
	//TODO return false if something went wrong or if there are no results
	return true;
}

QList<UPnP::UPnPHash> UPnP::goThroughDeviceList(UPNPDev *list)
{
    QList<UPnPHash> myResults;

    for (UPNPDev *l = list; l; l = l->pNext)
    {
        UPNPUrls urls;
        IGDdatas data;
        char lanaddr[64];
        UPnPHash resultHash;
        if(UPNP_GetValidIGD(l, &urls, &data, lanaddr, sizeof(lanaddr)) > 0)
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
                qDebug() << friendlyName;// + modelName + manufacturer;

                ClearNameValueList(&pdata);
            }
        }
        /* These URLs will be needed for accessing and controlling Mediaservers with SOAP */
        QString controlURL = data.tmp.controlurl;
        if(!controlURL.isEmpty())
        {
            resultHash.insert(ControlURL, controlURL);
        }

        QString eventSubUrl = data.tmp.eventsuburl;
        if(!eventSubUrl.isEmpty())
        {
            resultHash.insert(EventSubUrl, eventSubUrl);
        }

        QString scpdUrl = data.tmp.scpdurl;
        if(!scpdUrl.isEmpty())
        {
            resultHash.insert(ScpdURL, scpdUrl);
        }

        QString serviceType = data.tmp.servicetype;
        if(!serviceType.isEmpty())
        {
            resultHash.insert(ScpdURL, serviceType);
        }

        QString url = urls.controlURL;
        if(!url.isEmpty())
        {
            resultHash.insert(URL, url);
        }
        QString rootDescURL = urls.rootdescURL;
        if(!rootDescURL.isEmpty())
        {
            resultHash.insert(RootDescURL, rootDescURL);
        }
        FreeUPNPUrls(&urls);
        results.append(resultHash);
        myResults.append(resultHash);
    }
    freeUPNPDevlist(list);
    return myResults;
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
