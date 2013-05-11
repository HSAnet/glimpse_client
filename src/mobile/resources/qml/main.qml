import QtQuick 2.0
import mplane 1.0
import QtQuick.Controls 1.0
import QtQuick.Window 2.0

Rectangle {
    id: root
    width: 480
    height: 800

    SystemPalette {
        id: sysPal
    }

    color: sysPal.window

    property variant scheduler: client.scheduler

    Component.onCompleted: {
        // Initialize the client
        if ( !client.init() ) {
            console.log("Unable to initialize client");
            Qt.quit();
        }
    }

    Binding {
        target: statusBar
        property: "visible"
        value: client.scheduler.isStarted
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
    }

    Label {
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

    WebRequester {
        id: manualRequest
        request: TestRequest {
        }

        onStatusChanged: {
            switch(status) {
            case WebRequester.Running:
                statusText.text = qsTr("Requesting test ...");
                startButton.enabled = false;
                break;

            case WebRequester.Error:
                statusText.text = qsTr("Error: %1").arg(errorString());

            default:
                startButton.enabled = true;
            }

            console.log("Status changed: " + status);
        }
    }

    Button {
        id: startButton
        anchors.centerIn: parent
        text: qsTr("Start test")
        visible: !scheduler.isStarted       
        onClicked: manualRequest.start();
    }

    Label {
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
