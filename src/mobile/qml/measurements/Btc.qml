import QtQuick 2.1
import mplane 1.0
import "../controls"
import "../"

Base {
    id: root

    title: qsTr("Btc")

    measurement: "btc"
    properties: [
        {
            "label": "Host",
            "type": "TextField",
            "text": "141.82.57.241"
        },
    ]
}
