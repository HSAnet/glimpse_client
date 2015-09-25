import QtQuick 2.1
import mplane 1.0
import "../controls"
import "../"

Base {
    id: root

    title: qsTr("DNS Lookup")

    measurement: "dnsLookup"
    properties: [
        {
            "label": "Host",
            "type": "TextField",
            "text": "measure-it.net"
        },
        {
            "label": "DNS Server",
            "type": "TextField",
            "text": ""
        }

    ]
}

