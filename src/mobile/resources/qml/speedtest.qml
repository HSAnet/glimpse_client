import QtQuick 2.0

Item {
    id: root

    width: 400
    height: 400

    property string title: qsTr("Speedtest")
    property string subtitle: qsTr("downloading ...")
    property QtObject test

    Column {
        width: parent.width
        anchors.centerIn: parent
        spacing: 10

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Average speed: %1 kbyte/s").arg(test.averageDownloadSpeed / 1024)
            color: "white"
            font.pixelSize: 40
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Maximum speed: %1 kbyte/s").arg(test.maximumDownloadSpeed / 1024)
            color: "white"
            font.pixelSize: 40
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Progress: %1 %").arg(test.progress * 100)
            color: "white"
            font.pixelSize: 40
        }
    }
}

