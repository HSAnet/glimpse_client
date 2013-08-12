import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

TextFieldStyle {
    //textColor: "white"
    font.pixelSize: 28
    background: Item {
        implicitHeight: 50
        implicitWidth: 320
        BorderImage {
            source: "images/textinput.png"
            border.left: 8
            border.right: 8
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
        }
    }
}
