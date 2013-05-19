import QtQuick 2.0
import mplane 1.0
import QtQuick.Controls 1.0
import QtQuick.Window 2.0
import QtQuick.Layouts 1.0
import "android"

Rectangle {
    width: 400
    height: 500
    color: "lightgray"

    property string title: qsTr("Settings")
    property string subtitle: qsTr("all you want to change")

    Ping {
        id: pinger
        host: "8.8.8.8"
        onPing: {
            var line = {
                "time":time
            }

            model.append(line);
        }

        onStarted: model.clear()
    }

    Rectangle {
        id: card
        color: "white"
        border {
            width: 2
            color: "#626262"
        }

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            margins: 20
        }
        height: 200

        ColumnLayout {
            id: layout
            anchors {
                fill: parent
                margins: 10
            }

            spacing: 20

            Text {
                Layout.fillWidth: true
                text: pinger.status == Ping.NotRunning ? "Ping" : qsTr("Pinging %1 ...").arg(pinger.host)
                color: "gray"
                font.pointSize: 30

                MouseArea {
                    anchors.fill: parent
                    onClicked: pinger.start()
                }
            }

            GridView {
                Layout.fillWidth: true
                Layout.fillHeight: true

                cellWidth: 80
                cellHeight: 40

                height: 100

                delegate: Text {
                    color: "black"
                    font.pointSize: 18
                    text: time
                }

                model: ListModel {
                    id: model
                }
            }

            Text {
                Layout.fillWidth: true
                font.pointSize: 18
                color: "gray"
                text: qsTr("Average: %1 ms").arg(pinger.averagePingTime)
            }
        }
    }


    /*
    ListView {
        anchors.fill: parent

        header: Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Ping!"
            style: ButtonStyle {}
            onClicked: pinger.start()
        }

        model: ListModel {
            id: model
        }

        delegate: AndroidDelegate {
            text: title
        }
    }*/
}
