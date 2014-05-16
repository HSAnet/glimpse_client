import QtQuick 2.0
import mplane 1.0
import "controls"

ListPage {
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
        text: model.measurement
        onClicked: nextPage("measurements/%1.qml".arg(model.measurement))
    }

    Label {
        anchors {
            left: parent.left
            right: parent.right
            margins: units.gu(20)
        }

        y: (parent.height - parent.anchors.margins*2 - height - units.gu(20)) -contentY

        font.pixelSize: units.gu(25)
        text: qsTr("<b>Please note:</b> In this demo version all tasks add a delay of 10 seconds before the actual execution.")
        wrapMode: Text.WordWrap
    }
}
