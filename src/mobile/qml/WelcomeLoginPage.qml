import QtQuick 2.0
import "controls"

Page {
    id: root

    title: qsTr("First time login")
    activity: loginPage.activity

    Login {
        id: loginPage
        width: parent.width
        isWizard: true
    }

    Column {
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            bottomMargin: units.gu(50) + root.spacingValue
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
            text: qsTr("I don't have an account")
            onClicked: nextPage("WelcomeRegistrationPage.qml")
        }
    }
}
