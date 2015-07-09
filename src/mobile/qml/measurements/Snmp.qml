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

            ComboBox {
                id: measurementType

                width: 200
                model: ["Scan subnet", "Scan IP range", "Request OID"]
            }

            // Dialog to send a request
            Column {
                id: requestDialog

                Label {
                    text: qsTr("Choose version")
                }
                ComboBox {
                    id: snmpVersionComboBox
                    width: 200
                    model: ["SNMPv1", "SNMPv2c", "SNMPv3"]
                }

                Label {
                    text: qsTr("Host address")
                }
                TextField {
                    id: host
                    text: "host"
                }

                Label {
                    text: qsTr("Object Identifier")
                }
                TextField {
                    id: objectId
                    text: "oid"
                }

                // Community name dialog for request
                Column {
                    width: parent.width
                    visible: snmpVersionComboBox.currentIndex !== 2
                    Label {
                        text: qsTr("Community name")
                    }
                    TextField {
                        id: singleCommuniyName
                        text: "community"
                    }
                }

                // Security dialog for SNMPv3
                Column {
                    width: parent.width
                    visible: snmpVersionComboBox.currentIndex === 2
                    Label {
                        text: qsTr("Authentication")
                    }
                    ComboBox {
                        id: authAlgoCombo
                        model: ["none", "MD5"]
                    }
                    Label {
                        text: qsTr("Privacy")
                    }
                    ComboBox {
                        id: privacyCombo
                        model: ["none", "AES", "DES"]
                    }
                    Label {
                        text: qsTr("Username")
                    }
                    TextField {
                        id: usernameField
                    }
                    Label {
                        text: qsTr("Password")
                    }
                    TextField {
                        id: passwordField
                    }
                    Label {
                        text: qsTr("Context OID")
                    }
                    TextField {
                        id: contextOidField
                    }
                }
            }

            // Editfields to insert IP range
            Column {
                id: ipRangeDialog

                Label {
                    text: qsTr("Start IP of range")
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
            }

            // Slider to set send interval.
            Column {
                id: sendIntervalDialog

                Label {
                    text: qsTr("Send interval ms")
                }
                Row {
                    Slider {
                        id: intervalSlider
                        stepSize: 1
                        minimumValue: 2
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
            }

            // Set wait time after sending to receive answers.
            Column {
                id: waitTimeDialog

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
            }

            // Controls for a list of community names to scan.
            Column {
                id: communityList
                width: parent.width

                Label {
                    text: qsTr("Community name")
                }
                Row {
                    TextField {
                        id: communityName
                        width: 200
                    }
                    Item {
                        width: 20
                        height: 5
                    }
                    Button {
                        text: qsTr("add")
                        onClicked: {
                            if (communityName.text !== "") {
                                model.append({ name: communityName.text })
                            }
                        }
                    }
                }
                Item {
                    width: 5
                    height: 10
                }

                // ListView for community names
                Rectangle {
                    width: parent.width
                    height: 100
                    color: "white"
                    border.color: "black"

                    ListView {
                        id: list
                        anchors.fill: parent
                        anchors.margins: 10
                        model: model
                        delegate: delegate
                        highlight: itemHighlight
                    }
                }

                Button {
                    text: qsTr("remove")
                    onClicked: {
                        model.remove(list.currentIndex)
                    }
                }
            }

            Item {
                width: 5
                height: 10
            }

            // Button to start measurement.
            BigButton {
                id: startButton
                text: qsTr("Start")
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }

    ListModel {
        id: model
        ListElement { name: "public" }
        ListElement { name: "private" }
        ListElement { name: "demopublic" }
    }

    Component {
        id: delegate

        Text {
            width: list.width
            height: font.pixelSize + 6
            anchors.margins: 3
            anchors.leftMargin: 10
            text: " " + name

            MouseArea {
                anchors.fill: parent
                onClicked: list.currentIndex = index
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

    // States of view.
    states: [
        State {
            when: measurementType.currentIndex === 0
            PropertyChanges { target: ipRangeDialog; visible: false; }
            PropertyChanges { target: communityList; visible: true; }
            PropertyChanges { target: requestDialog; visible: false; }
        },
        State {
            when: measurementType.currentIndex === 1
            PropertyChanges { target: ipRangeDialog; visible: true; }
            PropertyChanges { target: communityList; visible: true; }
            PropertyChanges { target: requestDialog; visible: false; }
        },
        State {
            when: measurementType.currentIndex === 2
            PropertyChanges { target: ipRangeDialog; visible: false; }
            PropertyChanges { target: communityList; visible: false; }
            PropertyChanges { target: sendIntervalDialog; visible: false; }
            PropertyChanges { target: waitTimeDialog; visible: false; }
            PropertyChanges { target: requestDialog; visible: true; }
        }

    ]
}
