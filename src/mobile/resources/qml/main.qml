import QtQuick 2.0
import mplane 1.0
import QtQuick.Controls 1.0
import QtQuick.Window 2.0
import "android"

Rectangle {
    id: root
    width: 768
    height: 1200

    color: "#212126"

    property variant scheduler: client.scheduler

    Component.onCompleted: {
        // Initialize the client
        if ( !client.init() ) {
            console.log("Unable to initialize client");
            Qt.quit();
        }
    }

    // Implements back key navigation
    Keys.onReleased: {
        if (event.key === Qt.Key_Back) {
            if (pageStack.depth > 1) {
                pageStack.pop();
                event.accepted = true;
            } else { Qt.quit(); }
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
                    "test": currentTest
                }

                pageStack.push({item:Qt.resolvedUrl(currentTest.name + ".qml"), properties:params});
            }
        }
    }

    BorderImage {
        id: title
        border.bottom: 9
        source: "android/images/toolbar.png"
        width: parent.width
        height: 100
        z: 1

        BackButton {
            id: backButton
        }

        Text {
            id: applicationTitle
            font.pixelSize: 42
            Behavior on x { NumberAnimation{ easing.type: Easing.OutCubic} }
            x: backButton.x + backButton.width + 20
            anchors.verticalCenter: parent.verticalCenter
            color: "white"
            /*text: {
                var title = pageStack.currentItem.title;
                if (title)
                    return title;
                else
                    return "mPlane";
            }*/
        }

        Text {
            id: statusText
            font.pixelSize: 20
            x: applicationTitle.x + 30
            anchors.top: applicationTitle.bottom
            anchors.topMargin: -5
            color: "lightgray"
            /*text: {
                var subtitle = pageStack.currentItem.subtitle;
                if (subtitle)
                    return subtitle;
                else
                    return client.status == Client.Registered ? qsTr("Registered") : qsTr("Unregistered")
            }*/
        }
    }

    StackView {
        id: pageStack
        anchors {
            top: title.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        onCurrentItemChanged: {
            applicationTitle.text = currentItem.title;
            statusText.text = currentItem.subtitle;
        }

        initialItem: ListView {
            property string title: "mPlane"
            property string subtitle: client.status == Client.Registered ? qsTr("Registered") : qsTr("Unregistered")

            model: ListModel {
                ListElement {
                    title: "Manual tests"
                    page: "Tests.qml"
                }

                ListElement {
                    title: "Settings"
                    page: "Settings.qml"
                }
            }

            delegate: AndroidDelegate {
                text: title
                onClicked: pageStack.push(Qt.resolvedUrl(page))
            }

            footer: Button {
                id: startButton
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Start test")
                visible: !scheduler.isStarted
                enabled: !manualRequest.running
                onClicked: manualRequest.start()
                style: ButtonStyle {}
            }
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
                break;

            case WebRequester.Error:
                statusText.text = qsTr("Error: %1").arg(errorString());
                break;
            }
        }
    }
}
