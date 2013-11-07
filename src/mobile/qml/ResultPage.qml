import QtQuick 2.0

FlickablePage {
    title: qsTr("Report")

    property alias resultText: textView.text

    contentWidth: textView.width
    contentHeight: textView.height
    flickableDirection: Flickable.AutoFlickDirection

    Text {
        id: textView
        color: "black"
        font.family: "Roboto Light"
        font.pixelSize: units.gu(30)
        renderType: Text.NativeRendering
    }
}
