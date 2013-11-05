import QtQuick 2.0
import QtQuick.Controls 1.0
import "android"

Flickable {
    width: 400
    height: 500

    property string title: qsTr("Report")
    property string subtitle
    property alias resultText: textView.text

    flickableDirection: Flickable.VerticalFlick
    contentWidth: textView.width
    contentHeight: textView.height

    Text {
        id: textView
        width: parent.width
        color: "white"
        font.family: "Roboto Light"
        font.pixelSize: 30
        renderType: Text.NativeRendering
    }
}
