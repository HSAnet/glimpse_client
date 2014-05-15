import QtQuick 2.0
import mplane 1.0
import "controls"

Page {
    Rectangle {
        id: listBackground
        width: app.width - units.gu(50)
        height: parent.height

        anchors {
            top: parent.top
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
            topMargin: units.gu(20)
            bottomMargin: units.gu(20)
        }

        Image {
            id: flap
            anchors.right: parent.right
            anchors.top: parent.top
            source: "images/flap_small.png"
            height: units.gu(60)
            fillMode: Image.PreserveAspectFit
        }

        ListPage {
            anchors {
                fill: parent
                left: parent.left
            }
            title: qsTr("toolbox")
            subtitle: qsTr("see what's coming next")
            emptyText: qsTr("Your toolbox is empty")

            model: ListModel {
                ListElement {
                    title: "Ping"
                    group: "Simple measurements"
                    measurement: "Ping"
                }

                ListElement {
                    title: "UDP Ping"
                    group: "Simple measurements"
                    measurement: "UdpPing"
                }

                ListElement {
                    title: "Traceroute"
                    group: "Simple measurements"
                    measurement: "Traceroute"
                }

                ListElement {
                    title: "HTTP Download"
                    group: "Peer measurements"
                    measurement: "HttpDownload"
                }
            }

            section.delegate: Label {
                anchors {
                    left: parent.left
                    right: parent.right
                    leftMargin: units.gu(10)
                }
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignBottom
                text: section
                color: "#dea65a"
                font {
                    weight: Font.Normal
                    family: "Helvetica Neue"
                    pixelSize: units.gu(35)
                }
            }

            delegate: ListDelegate {
                showArrow: true
                showImage: false
                showBorder: true

                headline: model.title
                text: model.measurement
                onClicked: nextPage("measurements/%1.qml".arg(model.measurement))
            }
        }
    }
}
