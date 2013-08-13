import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

ButtonStyle {
    panel: Item {
        implicitHeight: 80
        implicitWidth: 520
        BorderImage {
            anchors.fill: parent
            antialiasing: true
            border.bottom: 8
            border.top: 8
            border.left: 8
            border.right: 8
            anchors.margins: control.pressed ? -4 : 0
            source: control.pressed ? "images/button_pressed.png" : "images/button_default.png"
            Text {
                text: control.text
                anchors.centerIn: parent
                color: "white"
                font.pixelSize: 50
                renderType: Text.NativeRendering
            }
        }
    }
}
