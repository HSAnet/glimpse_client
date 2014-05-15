import QtQuick 2.0

Page {
    title: qsTr("report")
    subtitle: qsTr("see what happened recently")

    property alias resultText: textView.text

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

        Text {
            id: textView
            color: "#333333"
            font.family: "Roboto Light"
            font.pixelSize: units.gu(20)
            renderType: Text.NativeRendering
        }
    }
}
