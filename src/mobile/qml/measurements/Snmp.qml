import QtQuick 2.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import "../controls"

Item {
    id: root
    property string title: "Snmp"
    property string subtitle: "Snmp tests"

    Column {
        width: parent.width - 40
        anchors.fill: parent
        anchors.margins: 20
        spacing: 10

        RadioButton {
            text: qsTr("Scan subnet")
        }

        RadioButton {
            text: qsTr("Scan range of IP addresses")
        }
        Label {
            text: qsTr("Start Ip of range")
        }
        TextField {
            width: 200
        }
        Label {
            text: qsTr("Last IP of range")
        }
        TextField {
            width: 200
        }

        Item {
            width: 10
            height: 1
        }

        Label {
            text: qsTr("Send interval ms")
        }
        Row {
            Slider {
                id: intervalSlider
                stepSize: 1
                minimumValue: 1
                maximumValue: 10
                value: 2
            }
            Item {
                width: 20
                height: 5
            }
            Label {
                text: qsTr("%1 ms").arg(intervalSlider.value)
            }
        }

        Label {
            text: qsTr("Wait for answers after scan")
        }
        Row {
            Slider {
                id: waitTimeSlider
                stepSize: 1
                minimumValue: 1
                maximumValue: 10
                value: 2
            }
            Item {
                width: 20
                height: 5
            }
            Label {
                text: qsTr("%1 seconds").arg(waitTimeSlider.value)
            }
        }

        Label {
            text: qsTr("Community name")
        }
        Row {
            TextField {
                width: 200
            }
            Item {
                width: 20
                height: 5
            }
            Button {
                text: qsTr("Add")
            }
        }
        Rectangle {
            width: parent.width
            height: 300
            color: "white"
            border.color: "black"

            ListView {
                id: communityList
                anchors.fill: parent
                anchors.margins: 10
                model: model
                delegate: delegate
                highlight: itemHighlight
            }
        }

        ListModel {
            id: model
            ListElement { name: "public" }
            ListElement { name: "private" }
            ListElement { name: "demopublic" }
        }
    }

    Component {
        id: delegate

            Text {
                width: communityList.width
                height: font.pixelSize + 6
                anchors.margins: 3
                anchors.leftMargin: 10
                text: name

            MouseArea {
                anchors.fill: parent
                onClicked: communityList.currentIndex = index
            }
        }
    }

    Component {
        id: itemHighlight

        Rectangle {
            gradient: Gradient {
                GradientStop { position: 0.0; color: "white" }
                GradientStop { position: 1.0; color: "lightsteelblue" }
            }
            radius: 3
            border.color: "lightblue"
            border.width: 1
        }
    }
}
