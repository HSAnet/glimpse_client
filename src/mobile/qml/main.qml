import QtQuick 2.0
import mplane 1.0
import "controls"

Rectangle {
    id: app
    width: units.gu(768)
    height: units.gu(1200)
    color: "#d1e0e0"

    Component.onCompleted: {
        // Initialize the client
        if ( !client.init(Client.ActiveProbe) ) {
            console.log("Unable to initialize client");
            Qt.quit();
        }

        // Maybe we can log in automatically from here
        if (client.autoLogin()) {
            nextPage("WelcomeLoginPage.qml");
        } else {
            nextPage("WelcomePage.qml");
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

    function menuPage() {
        var params = {
            item: Qt.resolvedUrl("MainPage.qml"),
        }

        if ( pageStack.depth > 0 ) {
            pageStack.insert(0, params);
            pageStack.pop(null);
        } else {
           nextPage(params);
        }
    }

    function nextPage(componentName, properties) {
        if (typeof(componentName) == "string") {
            var params = {
                item: Qt.resolvedUrl(componentName),
                properties: properties
            }

            pageStack.push(params);
        } else {
            pageStack.push(componentName);
        }
    }

    Binding {
        target: statusBar
        property: "visible"
        value: client.taskExecutor.running
    }

    Rectangle {
        color: "#f7f7f8"
        width: parent.width
        height: title.y + 2
        z: 1
        visible: Qt.platform.os == "ios"
    }

    Rectangle {
        id: title
        color: "#373737"
        y: Qt.platform.os == "ios" ? 20 : 0
        width: parent.width
        height: units.gu(45*2)
        z: 1

//        Rectangle {
//            width: parent.width
//            anchors.bottom: parent.bottom
//            height: 1 //units.gu(2)
//            color: "#b3b3b6"
//        }


        Image{
            id: settings
            anchors.right: parent.right
            source: "images/settings.png"
            anchors.verticalCenter: parent.verticalCenter
            height: 25; width: 25
            anchors.rightMargin: 20
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
            color: "#ffffff"
            font.weight: Font.Normal
            font.family: "Helvetica Neue"
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
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
    }
}
