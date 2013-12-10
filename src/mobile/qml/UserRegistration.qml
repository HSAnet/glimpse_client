import QtQuick 2.0
import mplane 1.0
import "controls"

Login {
    id: root

    title: qsTr("Registration")
    loginMode: false
    buttonTitle: qsTr("Register")

    function registerAnonymous() {
        client.loginController.anonymousRegistration();
    }

    Column {
        id: column
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            bottomMargin: units.gu(150)
        }

        width: parent.width
        spacing: units.gu(10)

        SpacerLine {
            anchors {
                left: parent.left
                right: parent.right
            }
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("I don't want to register")
            onClicked: root.registerAnonymous()
        }
    }
}
