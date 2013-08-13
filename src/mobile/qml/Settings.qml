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

    property string title: qsTr("NO INTERNET")
    property string subtitle: qsTr("checking your connection ...")

    Component.onDestruction: statusBar.visible = false

    Binding {
        target: statusBar
        property: "visible"
        value: tester.running || discovery.running || ping.status == Ping.Running
    }

    ListView {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        model: VisualItemModel {
            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                style: ButtonStyle {}
                text: qsTr("View results")
                visible: !(tester.running || discovery.running)
                onClicked: {
                    var results = [testerModel.result(),
                                   discovery.result(),
                                   ping.result()];

                    var params = {
                        item: Qt.resolvedUrl("ResultPage.qml"),
                        properties: {
                            subtitle: qsTr("Multiple results"),
                            resultText: JSON.stringify(results, null, "   ")
                        }
                    }

                    pageStack.push(params);
                }
            }

            Card {
                Component.onCompleted: tester.start()

                id: card

                title: "Checking Connection"
                width: ListView.view.width

                Behavior on x {
                    NumberAnimation {
                        duration: 100
                    }
                }

                opacity: (width-Math.abs(x))/width

                ListView {
                    id: listView
                    height: card.expanded ? 330 : 0
                    anchors {
                        left: parent.left
                        right: parent.right
                        leftMargin: 20
                    }
                    anchors.fill: card
                    interactive: false

                    model: ConnectionTesterModel {
                        id: testerModel
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

                    delegate: CardListDelegate {
                        leftText: model.testName
                        rightText: if (typeof(model.testResult) == "number") return model.testResult + " ms"; else return model.testResult;
                        showSpinner: !model.testFinished
                        showCheckmark: model.testFinished
                        checkmark: model.testSuccess
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
                            if ( hasData ) {
                                upnpModel.append( {"title":qsTr("Router Model"), "value":discovery.modelName} );
                                upnpModel.append( {"title":qsTr("Router Manufacturer"), "value":discovery.manufacturer} );
                                upnpModel.append( {"title":qsTr("LAN IP"), "value":discovery.lanIpAddress} );
                                upnpModel.append( {"title":qsTr("Public IP"), "value":discovery.externalIpAddress} );
                                upnpModel.append( {"title":qsTr("Link layer max. download"), "value":qsTr("%1 kbit/s").arg(discovery.linkLayerMaxDownload/1000)} );
                                upnpModel.append( {"title":qsTr("Link layer max. upload"), "value":qsTr("%1 kbit/s").arg(discovery.linkLayerMaxUpload/1000)} );
                            }
                        }
                    }
                }

                ListView {
                    x: 20
                    width: parent.width-20
                    height: card3.expanded ? 350 : 0
                    interactive: false
                    spacing: 5

                    model: ListModel {
                        id: upnpModel
                    }

                    delegate: CardListDelegate {
                        leftText: model.title
                        rightText: model.value
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

                    Row {
                        visible: !discovery.running && !discovery.hasData

                        Image {
                            source: "images/connectiontester_fail.png"
                        }

                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            text: qsTr("No UPNP data found")
                            font.pixelSize: 20
                            font.family: "Roboto Light"
                            color: "#707070"
                        }
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
