import QtQuick 2.0

Item {
    id: root

    width: 400
    height: 400

    property string title: qsTr("Packet train")
    property string subtitle: qsTr("Testing ...")
    property QtObject test

    Text {
        anchors.centerIn: parent
        text: qsTr("Packet %1 / 100 %2").arg(test.packetCount).arg(test.isMaster ? "received" : "sent")
        color: "white"
        font.pointSize: 30
    }
}
