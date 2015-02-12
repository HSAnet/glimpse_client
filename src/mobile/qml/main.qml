import QtQuick 2.0
import mplane 1.0
import analytics 1.0
import "controls"
import "."

Rectangle {
    id: app
    width: units.gu(768)
    height: units.gu(1200)
    color: "#d1e0e0"

    Component.onCompleted: {
        // Initialize the client
        if ( !client.init() ) {
            console.log("Unable to initialize client");
            Qt.quit();
        }

        // Maybe we can log in automatically from here
        if (client.autoLogin()) {
            autoLoginWatcher.enabled = true;
            menuPage();
        } else {
            nextPage("WelcomePage.qml");
        }
    }

    Component.onDestruction: {
        tracker.endSession();
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

    Tracker {
        id: tracker
        trackingID: "UA-51299738-2"
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

    Connections {
        id: autoLoginWatcher

        property bool enabled: false

        target: client.loginController
        onError: {
            if (enabled) {
                enabled = false;

                if (client.loginController.registeredDevice)
                    return;

                var params = {
                    item: Qt.resolvedUrl("WelcomeLoginPage.qml")
                }

                pageStack.insert(0, params);
                pageStack.pop(null);
            }
        }
        onFinished: {
            if (enabled) {
                enabled = false;

                if (client.loginController.registeredDevice)
                    return;

                var params = {
                    item: Qt.resolvedUrl("DeviceRegistration.qml"),
                    properties: {
                        "isWizard": true
                    }
                }

                pageStack.insert(0, params);
                pageStack.pop(null);
            }
        }
    }

    Binding {
        target: statusBar
        property: "visible"
        value: client.taskExecutor.running
    }

    /*transitions: [
        Transition {
            to: "settings"

            NumberAnimation {
                target: app
                properties: "x"
            }
        },

        Transition {
            to: ""

            SequentialAnimation {
                NumberAnimation {
                    target: app
                    property: "x"
                }
            }
        }
    ]*/

    /*states: State {
        name: "settings"

        PropertyChanges {
            target: app
            x: -app.width + units.gu(50)
        }
    }*/

    Rectangle {
        id: title
        color: "#373737"
        width: parent.width
        height: units.gu(45*2)
        z: 1

//        Rectangle {
//            width: parent.width
//            anchors.bottom: parent.bottom
//            height: 1 //units.gu(2)
//            color: "#b3b3b6"
//        }

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

        /*Button {
            id: actionTitle
            color: "white"

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
        }*/

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

        property variant analyticsTitle: {
            if (currentItem)
                return currentItem.analyticsTitle
            else
                return ""
        }

        onAnalyticsTitleChanged: {
            tracker.sendAppView(analyticsTitle);
        }
    }

    /*Rectangle {
        id: settingsPanel
        parent: app
        width: app.width
        color: "#333333"

        anchors {
            left: parent.right
            top: parent.top
            bottom: parent.bottom
        }

        Image{
            id: avatar
            source: "images/avatar.png"
            height: units.gu(150);
            width: units.gu(150)

            anchors {
                left: parent.left
                top: parent.top

                topMargin: units.gu(50)
                leftMargin: units.gu(30)
            }
        }

        Rectangle {
            anchors.fill: avatar
            color: "transparent"
            border.color: "white"
            border.width: units.gu(8)
        }

        Rectangle {
            id: settings_info
            anchors {
                left: avatar.right
                top: avatar.top

                leftMargin: units.gu(25)
            }

            Label {
                id: greeting
                text: qsTr("Hi, User.")
                font.pixelSize: units.gu(35)
                color: "white"

                anchors {
                    top: parent.top
                    topMargin: units.gu(20)
                }
            }

            Label {
                id: testcounter
                text: qsTr("Testcounter:    54")
                font.pixelSize: units.gu(27)
                color: "white"

                anchors {
                    top: greeting.bottom
                    topMargin: units.gu(15)
                }
            }

            Label {
                id: last_login
                text: qsTr("Last login:       12.2.2014")
                font.pixelSize: units.gu(27)
                color: "white"

                anchors {
                    top: testcounter.bottom
                    topMargin: units.gu(2)
                }
            }
        }

//        StackView {
//            id: stackView

//            anchors {
//                left: parent.left
//                right: parent.right
//                top: avatar.bottom
//                bottom: parent.bottom
//            }

//            initialItem: Settings {}
//        }

        ListModel {
            id: settingsModel

            ListElement {
                name: "Available Updates"
                size: "Settings"
            }
            ListElement {
                name: "Open as window"
                size: "Settings"
            }
            ListElement {
                name: "Preferences"
                size: "Settings"
            }
            ListElement {
                name: "Help"
                size: "Settings"
            }
            ListElement {
                name: "Change password"
                size: "User"
            }
            ListElement {
                name: "Logout"
                size: "User"
            }
            ListElement {
                name: "Quit"
                size: "Other"
            }
        }

        Component {
            id: sectionHeading

            Item {
                width: parent.width
                height: childrenRect.height

                Rectangle {
                    id: borderTop
                    anchors {
                        top: parent.top
                    }

                    width: parent.width
                    height: 1
                    color: "#7c7c7c"
                }

                Text {
                    id: section_element
                    text: section
                    font.pixelSize: units.gu(27)
                    color: "#7c7c7c"
                    anchors {
                        top: borderTop.bottom
                        topMargin: units.gu(5)
                        left: parent.left
                        leftMargin: units.gu(30)
                    }
                }

                Rectangle {
                    id: borderBottom
                    anchors {
                        top: section_element.bottom
                        topMargin: units.gu(5)
                    }

                    width: parent.width
                    height: 1
                    color: "#7c7c7c"
                }
            }

        }

        ListView {
            anchors {
                left: parent.left
                right: parent.right
                top: avatar.bottom
                bottom: parent.bottom

                topMargin: units.gu(50)
            }
            width: parent.width
            clip: true

            model: settingsModel
            delegate: Text {
                color: "white"
                text: name
                font.pixelSize: units.gu(35)
                anchors {
                    left: parent.left
                    leftMargin: units.gu(30)
                }
            }
            section.property: "size"
            section.criteria: ViewSection.FullString
            section.delegate: sectionHeading
        }

    }*/

}
