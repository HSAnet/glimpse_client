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
            "text": "http://www.measure-it.net/static/measurement/268435456"
        },

        {
            "label": "Avoid Caches",
            "type": "CheckBox",
            "checked": 1
        },

        {
            "label": "Threads",
            "type": "Slider",
            "stepSize": 1,
            "minimumValue": 1,
            "maximumValue": 6,
            "value": 4
        },

        {
            "label": "Target time",
            "type": "Slider",
            "stepSize": 250,
            "minimumValue": 2000,
            "maximumValue": 45000,
            "value": 10000,
            "format": qsTr("%1 ms")
        },

        {
            "label": "Ramp-up time",
            "type": "Slider",
            "stepSize": 500,
            "minimumValue": 1000,
            "maximumValue": 10000,
            "value": 3000,
            "format": qsTr("%1 ms")
        },

        {
            "label": "Slot length",
            "type": "Slider",
            "stepSize": 250,
            "minimumValue": 250,
            "maximumValue": 1000,
            "value": 1000,
            "format": qsTr("%1 ms")
        },

        {
            "label": "Source port",
            "type": "TextField",
            "text": ""
        }
    ]
}
