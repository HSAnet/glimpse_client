import QtQuick 2.0
import mplane 1.0
import "controls"

Rectangle {
    id: app
    width: units.gu(768)
    height: units.gu(1200)
    clip: true
    color: "#efeff4"

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
            if (pageStack.busy) {
                console.log("Ignoring back key while animating");
                event.accepted = true;
            }

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
        color: "#f7f7f8"
        width: parent.width
        height: 22
        z: 1
        visible: Qt.platform.os == "ios"
    }

    Rectangle {
        id: title
        color: "#f7f7f8"
        y: Qt.platform.os == "ios" ? 20 : 0
        width: parent.width
        height: units.gu(45*2)
        z: 1

        Rectangle {
            width: parent.width
            anchors.bottom: parent.bottom
            height: 1 //units.gu(2)
            color: "#b3b3b6"
        }

        BackButton {
            id: backButton

            anchors {
                left: parent.left
                leftMargin: units.gu(20)
                top: parent.top
                bottom: parent.bottom
                right: pageTitle.left
                rightMargin: units.gu(20)
            }

            arrowVisible: pageStack.depth > 1
            onClicked: pageStack.pop()

            text: {
                if (pageStack.depth == 1)
                    return "";

                var item = pageStack.get(pageStack.depth-2);
                if (item && item.title)
                    return item.title;
                else
                    return "";
            }
        }

        Label {
            id: pageTitle
            anchors.centerIn: parent
            font.bold: true
            //font.pixelSize: units.gu(45)
            text: {
                var item = pageStack.currentItem;
                if (item && item.title)
                    return item.title;
                else
                    return "";
            }
        }

        Button {
            id: actionTitle

            anchors {
                left: pageTitle.right
                leftMargin: units.gu(20)
                right: indicator.left
                rightMargin: units.gu(15)
                top: parent.top
                bottom: parent.bottom
            }

            Button {
                id: invisibleButton
                text: parent.text
                visible: false
            }
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignRight
            fontSizeMode: width < invisibleButton.width ? Text.HorizontalFit : Text.FixedSize

            Behavior on opacity {
                NumberAnimation {
                    duration: 250
                }
            }

            text: {
                var item = pageStack.currentItem;
                if (item && item.actionTitle)
                    return item.actionTitle;
                else
                    return "";
            }

            opacity: text.length ? 1 : 0
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

        initialItem: MenuPage {}
        //initialItem: WelcomePage {}
    }
}
