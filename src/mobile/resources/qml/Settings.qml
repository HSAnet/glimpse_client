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
            Component.onCompleted: tester.start()

            anchors.fill: parent

            spacing: 20

            Text {
                Layout.fillWidth: true
                text: "Connection tests"
            }

            ListView {
                id: listView
                spacing: 10
                height: 500
                Layout.fillWidth: true

                model: ConnectionTesterModel {
                    connectionTester: ConnectionTester {
                        id: tester
                    }
                }

                delegate: Item {
                    width: ListView.view.width
                    height: 70

                    RowLayout {
                        anchors.fill: parent

                        Text {
                            Layout.fillWidth: true
                            text: model.testName
                            font.pointSize: 18
                        }

                        Item {
                            height: 70
                            width: Math.max(70, resultText.width)

                            Spinner {
                                width: 65
                                anchors.centerIn: parent
                                running: !model.testFinished
                            }

                            Text {
                                id: resultText
                                anchors.centerIn: parent
                                text: if (typeof(model.testResult) == "number") return model.testResult + " ms"; else return model.testResult;
                                font.pointSize: 18
                                visible: model.testFinished
                            }
                        }
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
