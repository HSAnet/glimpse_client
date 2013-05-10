import QtQuick 2.0
import mplane 1.0
import QtQuick.Controls 1.0
import QtQuick.Window 2.0

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

    Component.onDestruction: {
        statusBar.hideIcon();
    }

    function packetCountChanged(packetCount) {
        packetText.text = qsTr("Packets: %1 / 100").arg(packetCount);
    }

    Connections {
        target: client.scheduler
        onCurrentTestChanged: {
            if ( !currentTest )
                //loader.sourceComponent = undefined;
                return;
            else {
                var params = {
                    "test": currentTest,
                    "color": "white"
                }

                loader.setSource(currentTest.name + ".qml", params);
            }
        }

        onIsStartedChanged: {
            if ( isStarted )
                statusBar.showIcon();
            else
                statusBar.hideIcon();
        }
    }

    Text {
        id: iconText

        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: 10
        }

        text: "ICON"
        font.family: "Sans Serif"
        font.pointSize: 30

        MouseArea {
            property int waas: 1;

            anchors.fill: parent
            onClicked: {
                statusBar.message = qsTr("Clicked %1 times").arg(waas);
                console.log(qsTr("message: %1").arg(statusBar.message));

                if ( waas++ % 2 )
                    statusBar.showIcon();
                else
                    statusBar.hideIcon();
            }
        }
    }

    Text {
        id: statusText
        anchors {
            top: parent.top
            topMargin: 10
            horizontalCenter: parent.horizontalCenter
        }

        text: client.status == Client.Registered ? qsTr("Registered") : qsTr("Unregistered")
    }

    Loader {
        id: loader
        anchors {
            top: statusText.bottom
            left: parent.left
            right: parent.right
            bottom: startButton.top
            margins: 10
        }
    }

    Button {
        id: startButton
        anchors.centerIn: parent
        text: qsTr("Start test")
        visible: !scheduler.isStarted       
        onClicked: client.requestTest()
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
