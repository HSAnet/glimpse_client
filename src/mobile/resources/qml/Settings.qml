import QtQuick 2.0
import mplane 1.0
import QtQuick.Controls 1.0
import QtQuick.Window 2.0
import QtQuick.Layouts 1.0
import "android"

Rectangle {
    id: root
    width: 400
    height: 500
    color: "#e5e5e5"

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

    BorderImage {
        id: card

        source: "images/card_background.png"
        horizontalTileMode: BorderImage.Repeat
        verticalTileMode: BorderImage.Repeat
        border {
            left: 15
            right: 15
            top: 15
            bottom: 15
        }

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            margins: 20
        }
        height: 530

        ColumnLayout {
            Component.onCompleted: tester.start()

            anchors.fill: parent
            anchors.margins: 10

            spacing: 20

            Text {
                Layout.fillWidth: true
                text: "Connection"
                font.family: "Roboto Light"
                color: "#707070"
                font.pixelSize: 50
            }

            ListView {
                id: listView
                //spacing: 10
                height: 300
                anchors.leftMargin: 20
                Layout.fillWidth: true
                Layout.fillHeight: true
                interactive: false

                model: ConnectionTesterModel {
                    connectionTester: ConnectionTester {
                        id: tester
                    }
                }

                add: Transition {
                    NumberAnimation {
                        properties: "opacity"
                        from: 0.0
                        to: 1.0
                        duration: 250
                    }
                }

                delegate: Item {
                    width: ListView.view.width
                    height: 70

                    Text {
                        anchors {
                            left: parent.left
                            leftMargin: 15
                            verticalCenter: parent.verticalCenter
                        }

                        text: model.testName
                        font.pixelSize: 30
                        font.family: "Roboto Light"
                    }

                    Item {
                        anchors {
                            right: checkMark.left
                            rightMargin: 10
                            verticalCenter: parent.verticalCenter
                        }

                        height: 50
                        width: Math.max(70, resultText.width)

                        Text {
                            id: resultText
                            anchors.centerIn: parent
                            text: if (typeof(model.testResult) == "number") return model.testResult + " ms"; else return model.testResult;
                            font.pixelSize: 20
                            font.family: "Roboto Light"
                            color: "#707070"
                            visible: model.testFinished && typeof(model.testResult) != "boolean"
                        }
                    }

                    Image {
                        id: checkMark

                        anchors {
                            right: parent.right
                            rightMargin: 10
                            verticalCenter: parent.verticalCenter
                        }

                        source: model.testSuccess ? "images/connectiontester_ok.png" : "images/connectiontester_fail.png"
                        visible: model.testFinished
                    }

                    Spinner {
                        width: 65
                        anchors {
                            right: parent.right
                            rightMargin: 10
                            verticalCenter: parent.verticalCenter
                        }
                        running: !model.testFinished
                    }
                }
            }
        }

        /*
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

                Spinner {
                    anchors {
                        right: parent.right
                        top: parent.top
                    }

                    width: parent.height
                    running: pinger.status == Ping.Running
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

                add: Transition {
                    NumberAnimation {
                        property: "scale"
                        from: 0.0
                        to: 1.0
                        duration: 250
                    }
                }

                remove: Transition {
                    NumberAnimation {
                        property: "x"
                        to: mapFromItem(root, root.width, 0).x
                        duration: 500
                    }
                }
            }

            Text {
                Layout.fillWidth: true
                font.pointSize: 18
                color: "gray"
                text: qsTr("Average: %1 ms").arg(pinger.averagePingTime)
            }
        }*/
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
