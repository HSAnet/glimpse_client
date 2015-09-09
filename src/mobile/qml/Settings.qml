import QtQuick 2.0
import QtQuick.Layouts 1.1
import mplane 1.0
import "controls"

Page {
    property string title: qsTr("Settings")

    Rectangle {
        anchors.fill: parent
        color: "#ffffff"

        Column {
            id: column
            spacing: 20
            anchors.top: parent.top
            anchors.topMargin: units.gu(50)
            anchors.left: parent.left
            anchors.right: parent.right


            Rectangle {
                id: hr
                height: units.gu(10)
                color: "#ff0000"
            }


            GridLayout {
                id: grid
                anchors {
                    left: parent.left
                    right: parent.right
                    leftMargin: units.gu(20)
                    rightMargin: units.gu(20)
                    //margins: units.gu(10)
                }
                columnSpacing: units.gu(50)
                columns: 2

                Label {
                    text: qsTr("Max. monthly traffic volume:\n%1MB").arg(trafficSlider.value)
                    wrapMode: Text.Wrap
                    font {
                        pixelSize: units.gu(30)
                    }
                    color: "#727277"
                    Layout.maximumWidth: Layout.minimumWidth
                }

                Slider {
                    id: trafficSlider
                    maximumValue: 1000.0
                    minimumValue: 30.0
                    value: client.settings.allowedTraffic / (1024 * 1024)
                    stepSize: 10
                    Layout.fillWidth: true
                }



                Label {
                    text: qsTr("Max. monthly traffic volume\n(mobile/cellular): %1MB").arg(trafficMobileSlider.value)
                    font {
                        pixelSize: units.gu(30)
                    }
                    color: "#727277"
                }

                Slider {
                    id: trafficMobileSlider
                    maximumValue: 500.0
                    minimumValue: 5.0
                    value: client.settings.allowedMobileTraffic / (1024 * 1024)
                    stepSize: 1
                    Layout.fillWidth: true
                }

                Label {
                    text: qsTr("Allow mobile/cellular\nmeasurements")
                    font {
                        pixelSize: units.gu(30)
                    }
                    color: "#727277"
                }

                CheckBox {
                    id: mobileMeasurementsCheckbox
                    checked: true
                }

                Label {
                    text: qsTr("Enable traffic budget\nmanager")
                    font {
                        pixelSize: units.gu(30)
                    }
                    color: "#727277"
                }

                CheckBox {
                    id: trafficBudgetManagerCheckbox
                    checked: true
                }

                Label {
                    text: qsTr("Keep logs for %1 days").arg(backlogSlider.value)
                    font {
                        pixelSize: units.gu(30)
                    }
                    color: "#727277"
                }

                Slider {
                    id: backlogSlider
                    maximumValue: 30.0
                    minimumValue: 0.0
                    value: 5
                    stepSize: 1
                    Layout.fillWidth: true
                }
            }

            BigButton {
                text: qsTr("Save")
                anchors.horizontalCenter: parent.horizontalCenter

                onClicked: {
                    client.settings.allowedTraffic = trafficSlider.value * 1024 * 1024
                    client.settings.allowedMobileTraffic = trafficMobileSlider.value * 1024 * 1024
                    client.settings.backlog = backlogSlider.value
                    client.settings.trafficBudgetManagerActive = trafficBudgetManagerCheckbox.checked
                    client.settings.mobileMeasurementsActive = mobileMeasurementsCheckbox.checked
                }
            }
        }
    }
}
