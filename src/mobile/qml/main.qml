import QtQuick 2.0
import mplane 1.0
import "controls"

Rectangle {
    id: root
    width: units.gu(768)
    height: units.gu(1200)
    clip: true

    Component.onCompleted: {
        // Initialize the client
        if ( !client.init() ) {
            console.log("Unable to initialize client");
            Qt.quit();
        }
    }

    // Implements back key navigation
    focus: true
    Keys.enabled: true
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
        value: client.taskExecutor.running
    }

    Rectangle {
        id: title
        color: "#f7f7f7"
        y: Qt.platform.os == "ios" ? 20 : 0
        width: parent.width
        height: units.gu(45*2)
        z: 1

        Rectangle {
            width: parent.width
            anchors.bottom: parent.bottom
            height: units.gu(2)
            color: "#d9d9d9"
        }

        BackButton {
            id: backButton

            anchors {
                left: parent.left
                leftMargin: units.gu(20)
                top: parent.top
                bottom: parent.bottom
            }

            arrowVisible: pageStack.depth > 1
            onClicked: pageStack.pop()
        }

        Text {
            id: actionTitle
            color: "#ff3b30"
            font.pixelSize: units.gu(45)
            anchors {
                right: parent.right
                rightMargin: units.gu(30)
                verticalCenter: parent.verticalCenter
            }

            visible: false
        }

        MouseArea {
            anchors {
                left: actionTitle.left
                right: parent.right
                top: parent.top
                bottom: parent.bottom
            }

            enabled: actionTitle.visible
            onClicked: pageStack.currentItem.actionClicked()
        }

        ActivityIndicator {
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
            topMargin: units.gu(5)
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        onCurrentItemChanged: {
            if (currentItem) {
                backButton.text = currentItem.title;

                if (currentItem.actionTitle) {
                    actionTitle.text = currentItem.actionTitle;
                    actionTitle.visible = true;
                }
                else
                    actionTitle.visible = false;

            } else {
                backButton.text = "";
                actionTitle.visible = false;
            }

            //statusText.text = currentItem ? currentItem.subtitle : "";
        }

        initialItem: MenuPage {
        }
    }
}
