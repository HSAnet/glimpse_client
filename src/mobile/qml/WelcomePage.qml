import QtQuick 2.0
import "controls"

Page {
    title: qsTr("Welcome to glimpse")

    Column {
        anchors {
            left: parent.left
            right: parent.right
            margins: units.gu(40)
        }

        Label {
            width: parent.width
            wrapMode: Text.Wrap
            text: "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet."
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Continue >")
            onClicked: nextPage("UserRegistration.qml")
        }
    }
}
