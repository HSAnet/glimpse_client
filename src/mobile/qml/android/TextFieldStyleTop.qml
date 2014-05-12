import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

TextFieldStyle {
    //textColor: "white"
    font.pixelSize: units.gu(35)
    textColor: "#333333"

    background: Item {
        implicitHeight: units.gu(92)
        implicitWidth: units.gu(652)
        BorderImage {
            source: "../images/input_top.png"

            width: parent.width
            height: parent.height

            border.left: 8
            border.right: 8
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right

            visible: !control.activeFocus
        }

        BorderImage {
            source: "../images/input_top.png"

            width: parent.width
            height: parent.height

            border.left: 8
            border.right: 8
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right

            visible: control.activeFocus
        }
    }
}
