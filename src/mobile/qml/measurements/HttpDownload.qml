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
            "text": "http://www.measure-it.net:80/static/measurement/67108864"
        },
    ]
}
