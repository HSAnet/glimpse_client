import QtQuick 2.0

Rectangle {
    id: root

    width: 400
    height: 400
    color: "gray"

    property QtObject test

    Text {
        anchors {
            top: parent.top
            topMargin: 10
            horizontalCenter: parent.horizontalCenter
        }

        text: qsTr("Packet train")
        font.bold: true
        font.pointSize: 16
    }

    Text {
        anchors.centerIn: parent
        text: qsTr("Packet %1 / 100 received").arg(test.packetCount)
    }
}
