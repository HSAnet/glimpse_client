import QtQuick 2.1
import mplane 1.0
import "../controls"
import "../"

Base {
    id: root

    title: qsTr("Packet Trains")

    measurement: "packetTrains"
    properties: [
        {
            "label": "Host",
            "type": "TextField",
            "text": "141.82.57.241"
        },
        {
            "label": "Port",
            "type": "Slider",
            "stepSize": 1,
            "minimumValue": 1025,
            "maximumValue": 65535,
            "value": 5106
        },
        {
            "label": "Packet size",
            "type": "Slider",
            "stepSize": 1,
            "minimumValue": 1,
            "maximumValue": 1450,
            "value": 1000,
            "format": qsTr("%1 bytes")
        },
        {
            "label": "Train length",
            "type": "Slider",
            "stepSize": 1,
            "minimumValue": 10,
            "maximumValue": 128,
            "value": 48
        },
        {
            "label": "Iterations",
            "type": "Slider",
            "stepSize": 1,
            "minimumValue": 1,
            "maximumValue": 16,
            "value": 1
        },
        {
            "label": "Minimum rate",
            "type": "Slider",
            "stepSize": 1024,
            "minimumValue": 1024,
            "maximumValue": 262144000,
            "value": 10485760,
            "format": qsTr("%1 byte/s")
        },
        {
            "label": "Maximum rate",
            "type": "Slider",
            "stepSize": 1024,
            "minimumValue": 1024,
            "maximumValue": 262144000,
            "value": 262144000,
            "format": qsTr("%1 byte/s")
        },
        {
            "label": "Delay",
            "type": "Slider",
            "stepSize": 1000,
            "minimumValue": 1000000,
            "maximumValue": 1000000000,
            "value": 200000000,
            "format": qsTr("%1 ns")
        },
    ]
}
