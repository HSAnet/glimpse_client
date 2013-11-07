import QtQuick 2.0
import "controls"

FlickablePage {
    title: qsTr("Welcome to glimpse")

    Column {
        anchors {
            left: parent.left
            right: parent.right
            margins: units.gu(40)
        }

        Item { width: 1; height: units.gu(100) }

        Label {
            width: parent.width
            wrapMode: Text.Wrap
            text: "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet."
        }

        Item { width: 1; height: units.gu(50) }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Continue >")
            onClicked: nextPage("UserRegistration.qml")
        }
    }
}
