import QtQuick 2.1
import mplane 1.0
import "../controls"
import "../"

Base {
    id: root

    title: qsTr("HTTP Download")

    measurement: "http"
    properties: [
        {
            "label": "Url",
            "type": "TextField",
            "text": "http://ipv4.download.thinkbroadband.com:81/10MB.zip"
        },
    ]
}
