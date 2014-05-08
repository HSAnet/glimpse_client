import QtQuick 2.0
import "controls"

ListPage {
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
            rightMargin: 10
        }
        height: headerHeight
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignBottom
        text: section
        color: "gray"
    }

    delegate: ListDelegate {
        text: model.title
        onClicked: nextPage("measurements/%1.qml".arg(model.measurement))
    }
}
