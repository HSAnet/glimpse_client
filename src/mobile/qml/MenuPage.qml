import QtQuick 2.0
import mplane 1.0
import "android"
import "controls"

Page {
    title: "Glimpse"
    actionTitle: qsTr("Settings")

    function actionClicked() {
        nextPage("Settings.qml");
    }

    Column {
        id: column
        width: parent.width
        spacing: units.gu(50)

        Button {
            text: "Registration"
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: nextPage("UserRegistration.qml")
        }

        Button {
            text: "Scheduler"
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: nextPage("SchedulerPage.qml")
        }

        Button {
            text: "Reports"
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: nextPage("ReportsPage.qml")
        }

        Button {
            text: "Internet is not working"
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: nextPage("Settings.qml")
        }

        Button {
            text: "Internet is slow"
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: nextPage("Slow.qml")
        }

        Button {
            text: "Btc"
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: client.btc()
        }

        Button {
            text: "Upnp"
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: client.upnp()
        }

        Button {
            text: "Ping"
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: client.ping()
        }
    }
}
