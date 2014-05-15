import QtQuick 2.1
import mplane 1.0
import "../controls"
import "../"

Base {
    id: root

    title: qsTr("ping")

    measurement: "ping"
    properties: [
        {
            "label": "Host",
            "type": "TextField",
            "text": "google.com"
        },

        {
            "label": "Count",
            "type": "Slider",
            "stepSize": 1,
            "minimumValue": 1,
            "maximumValue": 255,
            "value": 4
        },

        {
            "label": "Timeout",
            "type": "Slider",
            "stepSize": 1,
            "minimumValue": 25,
            "maximumValue": 20000,
            "value": 2000,
            "format": qsTr("%1 milliseconds")
        },

        {
            "label": "Interval",
            "type": "Slider",
            "stepSize": 1,
            "minimumValue": 25,
            "maximumValue": 10000,
            "value": 200,
            "format": qsTr("%1 milliseconds")
        }
    ]
}
