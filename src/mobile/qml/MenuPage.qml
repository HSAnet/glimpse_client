import QtQuick 2.0
import QtQuick.Controls 1.0
import mplane 1.0
import "android"

Page {
    title: "Glimpse"
    subtitle: client.status == Client.Registered ? qsTr("Logged in") : qsTr("Please log in")

    Column {
        id: column
        width: parent.width
        spacing: units.gu(50)

        Button {
            text: "Registration"
            anchors.horizontalCenter: parent.horizontalCenter
            style: ButtonStyle {}
            onClicked: pageStack.push(Qt.resolvedUrl("UserRegistration.qml"))
        }

        Button {
            text: "Scheduler"
            anchors.horizontalCenter: parent.horizontalCenter
            style: ButtonStyle {}
            onClicked: pageStack.push(Qt.resolvedUrl("SchedulerPage.qml"))
        }

        Button {
            text: "Reports"
            anchors.horizontalCenter: parent.horizontalCenter
            style: ButtonStyle {}
            onClicked: pageStack.push(Qt.resolvedUrl("ReportsPage.qml"))
        }

        Button {
            text: "Internet is not working"
            anchors.horizontalCenter: parent.horizontalCenter
            style: ButtonStyle {}
            onClicked: pageStack.push(Qt.resolvedUrl("Settings.qml"))
        }

        Button {
            text: "Internet is slow"
            anchors.horizontalCenter: parent.horizontalCenter
            style: ButtonStyle {}
            onClicked: pageStack.push(Qt.resolvedUrl("Slow.qml"))
        }

        Button {
            text: "Btc"
            anchors.horizontalCenter: parent.horizontalCenter
            style: ButtonStyle {}
            onClicked: client.btc()
        }

        Button {
            text: "Upnp"
            anchors.horizontalCenter: parent.horizontalCenter
            style: ButtonStyle {}
            onClicked: client.upnp()
        }

        Button {
            text: "Ping"
            anchors.horizontalCenter: parent.horizontalCenter
            style: ButtonStyle {}
            onClicked: client.ping()
        }
    }
}
