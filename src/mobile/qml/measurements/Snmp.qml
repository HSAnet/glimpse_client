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

        LabeledComboBox {
            id: measurementType
            label: qsTr("Mode")
            model: ["Scan subnet", "Scan IP range", "Request a device"]
        }

        // -----------------------
        // Dialog to send a request
        Column {
            id: requestDialog
            width: parent.width
            spacing: 5

            LabeledComboBox {
                id: snmpVersionComboBox
                label: qsTr("Version")
                model: ["SNMPv1", "SNMPv2c", "SNMPv3"]
            }
            LabeledTextField {
                id: host
                label: qsTr("Host")
            }
            LabeledTextField {
                id: objectId
                label: qsTr("Object Id")
            }
            // Community name dialog for request
            LabeledTextField {
                id: singleCommunityName
                label: qsTr("Community")
                visible: snmpVersionComboBox.currentIndex !== 2
            }
            // Security dialog for SNMPv3
            Column {
                width: parent.width
                spacing: 5
                visible: snmpVersionComboBox.currentIndex === 2

                LabeledComboBox {
                    id: authAlgoCombo
                    label: qsTr("Authentication")
                    controlScale: 0.3
                    model: ["none", "MD5"]
                }
                LabeledComboBox {
                    id: privacyAlgoCombo
                    label: qsTr("Privacy")
                    controlScale: 0.3
                    model: ["none", "AES", "DES"]
                }
                LabeledTextField {
                    id: usernameField
                    label: qsTr("Username")
                }
                LabeledTextField {
                    id: passwordField
                    label: qsTr("Password")
                }
                LabeledTextField {
                    id: contextOidField
                    label: qsTr("Context Oid")
                }
            }
        }

        // -----------------------------
        // Editfield to insert IP range
        LabeledTextField {
            id: ipRangeDialog
            label: qsTr("IP range")
            text: qsTr("startIP-endIP")
        }

        // ---------------------------------------------------------------------
        // Slider to set send interval, wait time for answers and retries per IP.
        Column {
            id: timingAndRetryDialog
            spacing: 5

            Label {
                text: qsTr("Send interval ms")
            }
            Row {
                spacing: 20
                Slider {
                    id: intervalSlider
                    stepSize: 1
                    minimumValue: 2
                    maximumValue: 10
                    value: 2
                }
                Label {
                    text: qsTr("%1 ms").arg(intervalSlider.value)
                }
            }
            Label {
                text: qsTr("Wait for answers after scan")
            }
            Row {
                spacing: 20
                Slider {
                    id: waitTimeSlider
                    stepSize: 1
                    minimumValue: 1
                    maximumValue: 10
                    value: 2
                }
                Label {
                    text: qsTr("%1 seconds").arg(waitTimeSlider.value)
                }
            }
            Label {
                text: qsTr("Retries per IP")
            }
            Row {
                spacing: 20
                Slider {
                    id: retrySlider
                    stepSize: 1
                    minimumValue: 1
                    maximumValue: 5
                    value: 2
                }
                Label {
                    text: qsTr("%1 retries").arg(retrySlider.value)
                }
            }
        }

        // -----------------------------------------------
        // Controls to maintain a list of community names.
        Column {
            id: communityList
            width: parent.width
            spacing: 5

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

            // ListView for community names
            Rectangle {
                width: parent.width
                height: 80
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

        // Button to start measurement.
        BigButton {
            id: startButton
            text: qsTr("Start")
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: startMeasurement()
        }
    }

    // -----------------------------------------
    // List utility : model, delegate, highliting
    ListModel {
        id: model

        readonly property var stringList: {
            var list = []
            for (var i=0; i<count; ++i) {
                list.push(get(i).name)
            }

            return list
        }

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

    // ------------------------------------
    // Function :       startMeasurement()
    function startMeasurement() {
        if (measurementType === 2) {
            var snmpVersion3 = 3;
            var communityList = [singleCommunityName.text]
            client.snmp(snmpVersion3, communityList, host.text, objectId.text, authAlgoCombo.currentIndex,
                        privacyAlgoCombo.currentIndex, usernameField.text, passwordField.text, contextOidField.text)
        } else {
            client.snmp(model.stringList, retrySlider.value, snmpVersionComboBox.currentIndex, ipRangeDialog.text,
                        measurementType.currentIndex, intervalSlider.value, waitTimeSlider.value)
        }
    }

    // -------------------------
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
            PropertyChanges { target: timingAndRetryDialog; visible: false; }
            PropertyChanges { target: requestDialog; visible: true; }
        }

    ]
}
