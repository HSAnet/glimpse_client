import QtQuick 2.0
import mplane 1.0

Rectangle {
    id: root
    width: 480
    height: 800

    property variant scheduler: client.scheduler

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
        id: startButton
        anchors.centerIn: parent
        text: qsTr("Start test")
        visible: !scheduler.isStarted

        MouseArea {
            anchors.fill: parent
            onClicked: client.requestTest()
        }
    }

    Text {
        id: activeText

        anchors.centerIn: parent

        RotationAnimation {
            target: activeText
            direction: RotationAnimation.Clockwise
            from: 0
            to: 360
            duration: 1000
            loops: Animation.Infinite
            alwaysRunToEnd: true
            running: scheduler.isStarted
        }

        Behavior on scale {
            NumberAnimation {
                duration: 500
            }
        }

        scale: scheduler.isStarted ? 1.0 : 0.0
        text: qsTr("Testing")
    }
}
