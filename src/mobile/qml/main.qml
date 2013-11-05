import QtQuick 2.0
import mplane 1.0
import QtQuick.Controls 1.0
import QtQuick.Window 2.0
import "android"

Rectangle {
    id: root
    width: units.gu(768)
    height: units.gu(1200)
    clip: true

    color: "#212126"

    property variant scheduler: client.scheduler

    Component.onCompleted: {
        // Initialize the client
        if ( !client.init() ) {
            console.log("Unable to initialize client");
            Qt.quit();
        }
    }

    Behavior on scale {
        NumberAnimation {
            easing.type: Easing.OutBack
            duration: 100
        }
    }

    // Implements back key navigation
    focus: true
    Keys.enabled: true
    Keys.onReleased: {
        if (event.key === Qt.Key_Plus)
            root.scale += 0.1
        if (event.key === Qt.Key_Minus)
            root.scale -= 0.1

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
        value: client.taskExecutor.running
    }

    Connections {
        target: client.scheduler
        /*onCurrentTestChanged: {
            if ( !currentTest )
                //loader.sourceComponent = undefined;
                return;
            else {
                var params = {
                    "test": currentTest
                }

                pageStack.push({item:Qt.resolvedUrl(currentTest.name + ".qml"), properties:params});
            }
        }*/
    }

    BorderImage {
        id: title
        border.bottom: 9
        source: "android/images/toolbar.png"
        width: parent.width
        height: applicationTitle.height * 2
        z: 1

        BackButton {
            id: backButton
        }

        Text {
            id: applicationTitle
            font.pixelSize: units.gu(55)
            Behavior on x { NumberAnimation{ easing.type: Easing.OutCubic} }
            x: backButton.x + backButton.width + units.gu(20)
            anchors {
                top: parent.top
                topMargin: units.gu(10)
            }
            color: "white"
        }

        Text {
            id: statusText
            font.pixelSize: units.gu(35)
            x: applicationTitle.x + units.gu(30)
            anchors.top: applicationTitle.bottom
            anchors.topMargin: units.gu(-5)
            color: "lightgray"
        }

        Spinner {
            anchors {
                right: parent.right
                rightMargin: units.gu(20)
                verticalCenter: parent.verticalCenter
            }

            running: client.taskExecutor.running
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

        /*
        delegate: StackViewDelegate {
            function transitionFinished(properties)
            {
                properties.exitItem.x = 0
            }

            property Component pushTransition: StackViewTransition {
                PropertyAnimation {
                    target: enterItem
                    property: "x"
                    from: enterItem.width
                    to: 0
                    duration: 100
                }
                PropertyAnimation {
                    target: exitItem
                    property: "x"
                    from: 0
                    to: -exitItem.width
                    duration: 100
                }
            }

            property Component popTransition: StackViewTransition {
                PropertyAnimation {
                    target: enterItem
                    property: "x"
                    from: -enterItem.width
                    to: 0
                    duration: 100
                }
                PropertyAnimation {
                    target: exitItem
                    property: "x"
                    from: 0
                    to: exitItem.width
                    duration: 100
                }
            }
        }
        */

        onCurrentItemChanged: {
            applicationTitle.text = currentItem ? currentItem.title : "";
            statusText.text = currentItem ? currentItem.subtitle : "";
        }

        initialItem: Flickable {
            property string title: "Glimpse"
            property string subtitle: client.status == Client.Registered ? qsTr("Logged in") : qsTr("Please log in")

            width: parent.width
            height: parent.height

            flickableDirection: Flickable.VerticalFlick
            //contentWidth: column.width
            contentHeight: column.height

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
    }
}
