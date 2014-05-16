import QtQuick 2.1
import mplane 1.0
import "../controls"
import "../"

Base {
    id: root

    title: qsTr("Packet Trains")

    measurement: "packettrains"
    properties: [
        {
            "label": "Host",
            "type": "TextField",
            "text": "141.82.57.241"
        },
        {
            "label": "Port",
            "type": "TextField",
            "text": "5106"
        },
        {
            "label": "PacketSize",
            "type": "TextField",
            "text": "1000"
        },
        {
            "label": "TrainLength",
            "type": "TextField",
            "text": "48"
        },
        {
            "label": "Iterations",
            "type": "TextField",
            "text": "1"
        },
        {
            "label": "RateMin",
            "type": "TextField",
            "text": "10485760"
        },
        {
            "label": "RateMax",
            "type": "TextField",
            "text": "262144000"
        },
        {
            "label": "Delay",
            "type": "TextField",
            "text": "200000000"
        },
    ]
}
