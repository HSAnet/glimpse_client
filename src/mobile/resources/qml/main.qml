import QtQuick 2.0
import mplane 1.0

Rectangle {
    width: 480
    height: 800

    Component.onCompleted: {
        // Initialize the client
        if ( !client.init() ) {
            console.log("Unable to initialize client");
            Qt.quit();
        }
    }

    Text {
        anchors {
            top: parent.top
            topMargin: 10
            horizontalCenter: parent.horizontalCenter
        }

        text: client.status == Client.Registered ? qsTr("Registered") : qsTr("Unregistered")
    }

    Text {
        anchors.centerIn: parent
        text: qsTr("Start test")

        MouseArea {
            anchors.fill: parent
            onClicked: client.requestTest()
        }
    }
}
