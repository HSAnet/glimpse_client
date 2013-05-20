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

    property string title: qsTr("Manual tests")
    property string subtitle: qsTr("running your choice")

    ListView {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        model: VisualItemModel {
            Card {
                Component.onCompleted: tester.start()

                id: card

                title: "Checking Connection"
                width: ListView.view.width

                ListView {
                    id: listView
                    height: card.expanded ? 440 : 0
                    anchors {
                        left: parent.left
                        right: parent.right
                        leftMargin: 20
                    }
                    anchors.fill: card
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

            Card {
                id: card3
                title: "UPNP Discovery"
                width: ListView.view.width

                Component.onCompleted: discovery.discover()

                Item {
                    Discovery {
                        id: discovery

                        onStarted: upnpModel.clear()
                        onFinished: {
                            upnpModel.append( {"title":qsTr("LAN IP: %1").arg(discovery.lanIpAddress)} );
                            upnpModel.append( {"title":qsTr("External IP: %1").arg(discovery.externalIpAddress)} );
                            upnpModel.append( {"title":qsTr("Link layer max. download: %1 kbit/s").arg(discovery.linkLayerMaxDownload/1000)} );
                            upnpModel.append( {"title":qsTr("Link layer max. upload: %1 kbit/s").arg(discovery.linkLayerMaxUpload/1000)} );
                        }
                    }
                }

                ListView {
                    width: parent.width
                    height: card3.expanded ? 150 : 0
                    interactive: false
                    spacing: 5

                    model: ListModel {
                        id: upnpModel
                    }

                    delegate: Text {
                        text: model.title
                        font.pixelSize: 20
                        font.family: "Roboto Light"
                        color: "#707070"
                    }

                    Spinner {
                        anchors {
                            right: parent.right
                            bottom: parent.bottom
                            margins: 30
                        }

                        width: 65
                        running: discovery.running
                    }
                }
            }

            Card {
                id: card2
                title: qsTr("Pinging")
                width: ListView.view.width

                Component.onCompleted: ping.start()

                Item {
                    Ping {
                        id: ping
                        host: "mplane.informatik.hs-augsburg.de"
                        amount: 20

                        onStarted: listModel.clear()
                        onPing: {
                            var row = {
                                "title": time
                            }

                            listModel.append(row);
                        }
                    }
                }

                GridView {
                    width: parent.width
                    height: card2.expanded ? 150 : 0

                    cellWidth: 50
                    cellHeight: 20
                    interactive: false

                    model: ListModel {
                        id: listModel
                    }

                    delegate: Text {
                        text: model.title
                        font.pixelSize: 20
                        font.family: "Roboto Light"
                        color: "#707070"
                    }

                    Spinner {
                        anchors {
                            right: parent.right
                            bottom: parent.bottom
                            margins: 30
                        }

                        width: 65
                        running: ping.status == Ping.Running
                    }
                }
            }
        }
    }
}
