import QtQuick 2.0
import mplane 1.0
import "controls"

ListPage {
    title: qsTr("toolbox")
    subtitle: qsTr("see what's coming next")
    emptyText: qsTr("Your toolbox is empty")

    model: ListModel {
        ListElement {
            title: "UPnP"
            group: "Simple measurements"
            measurement: "Upnp"
            description: "Get information from local UPnP devices"
        }

        ListElement {
            title: "Ping"
            group: "Public peer measurements"
            measurement: "Ping"
            description: "Latency measurement"
        }

        ListElement {
            title: "Traceroute"
            group: "Public peer measurements"
            measurement: "Traceroute"
            description: "Measures the path to a certain host with UDP"
        }

        ListElement {
            title: "HTTP download"
            group: "Public peer measurements"
            measurement: "HttpDownload"
            description: "Download something from a public website"
        }

        ListElement {
            title: "Btc"
            group: "Glimpse peer measurements"
            measurement: "Btc"
            description: "Bulk transfer capacity measurement (TCP)"
        }

        ListElement {
            title: "Packet Trains"
            group: "Glimpse peer measurements"
            measurement: "Packettrains"
            description: "Trains of packet pairs measurement"
        }
    }

    section.delegate: Rectangle {
        width: parent.width
        height: sectionLabel.height

        Label {
            id: sectionLabel
            anchors {
                left: parent.left
                right: parent.right
                leftMargin: units.gu(20)
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
    }

    delegate: ListDelegate {
        showArrow: true
        showImage: false

        headline: model.title
        text: model.description
        onClicked: nextPage("measurements/%1.qml".arg(model.measurement))
    }

    footer: Rectangle {
        width: parent.width
        height: note.height

        Label {
            id: note
            anchors {
                left: parent.left
                right: parent.right
                margins: units.gu(20)
            }
        }
    }
}
