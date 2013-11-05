import QtQuick 2.1
import QtQuick.Controls 1.0
import mplane 1.0

Rectangle {
    id: backButton
    width: opacity ? units.gu(40) : 0
    anchors.left: parent.left
    anchors.leftMargin: units.gu(20)
    opacity: pageStack.depth > 1 ? 1 : 0
    anchors.verticalCenter: parent.verticalCenter
    antialiasing: true
    height: units.gu(60)
    radius: units.gu(4)
    //color: backmouse.pressed ? "#222" : "transparent"
    color: "transparent"

    Behavior on opacity { NumberAnimation{} }
    /*Image {
        anchors.verticalCenter: parent.verticalCenter
        source: "images/navigation_previous_item.png"
    }*/

    Text {
        anchors {
            right: parent.right
            verticalCenter: parent.verticalCenter
        }
        text: "<"
        color: "#ff3b30"
        font.pixelSize: units.gu(45)
    }

    MouseArea {
        id: backmouse
        anchors.fill: parent
        anchors.margins: units.gu(-10)
        onClicked: pageStack.pop()
    }
}
