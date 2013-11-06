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

    function nextPage(componentName, properties) {
        var params = {
            item: Qt.resolvedUrl(componentName),
            properties: properties
        }

        pageStack.push(params);
    }

    Binding {
        target: statusBar
        property: "visible"
        value: client.taskExecutor.running
    }

    Rectangle {
        color: "#f7f7f7"
        width: parent.width
        height: 22
        z: 1
        visible: Qt.platform.os == "ios"
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
            height: 1 //units.gu(2)
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

        Button {
            id: actionTitle

            anchors {
                right: indicator.left
                rightMargin: units.gu(15)
                verticalCenter: parent.verticalCenter
            }

            Behavior on opacity {
                NumberAnimation {
                    duration: 250
                }
            }

            opacity: 0.0
            onClicked: pageStack.currentItem.actionClicked()
        }

        ActivityIndicator {
            id: indicator

            anchors {
                right: parent.right
                rightMargin: units.gu(15)
                verticalCenter: parent.verticalCenter
            }

            width: running ? implicitWidth : 0
            running: {
                if (client.taskExecutor.running)
                    return true;

                var item = pageStack.currentItem;
                if (item && item.activity)
                    return true;

                return false;
            }
            visible: running

            Behavior on width {
                NumberAnimation {
                    easing.type: Easing.OutCubic
                }
            }
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
                    actionTitle.opacity = 1;
                }
                else
                    actionTitle.opacity = 0;

            } else {
                backButton.text = "";
                actionTitle.opacity = 0;
            }
        }

        initialItem: MenuPage {}
        //initialItem: WelcomePage {}
    }
}
