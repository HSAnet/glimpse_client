import QtQuick 2.0
import QtQuick.Controls 1.0
import "android"

Page {
    title: qsTr("Report")

    property alias resultText: textView.text

    Text {
        id: textView
        width: parent.width
        color: "black"
        font.family: "Roboto Light"
        font.pixelSize: units.gu(30)
        renderType: Text.NativeRendering
    }
}
