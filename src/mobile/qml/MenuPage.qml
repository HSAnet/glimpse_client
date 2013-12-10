import QtQuick 2.0
import mplane 1.0 as Mplane
import "controls"

ListPage {
    id: root
    title: "Glimpse"
    actionTitle: qsTr("Settings")

    function actionClicked() {
        if (root.state == "settings")
            root.state = "";
        else
            root.state = "settings";
    }

    Rectangle {
        id: settingsPanel
        parent: app
        width: app.width - units.gu(100)
        color: "darkgray"

        anchors {
            left: parent.right
            top: parent.top
            bottom: parent.bottom
        }

        Label {
            id: label
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Settings")
        }

        StackView {
            id: stackView

            anchors {
                left: parent.left
                right: parent.right
                top: label.bottom
                bottom: parent.bottom
            }

            initialItem: Settings {}
        }
    }

    states: State {
        name: "settings"

        PropertyChanges {
            target: app
            x: -root.width + units.gu(100)
        }
    }

    transitions: Transition {
        NumberAnimation {
            properties: "x"
            easing.type: Easing.InOutCubic
            duration: 150
        }
    }

    delegate: ListDelegate {
        text: model.text
        onClicked: {
            if (model.url)
                nextPage(model.url);
            else {
                var func = client[model.func];
                if (func)
                    func();
                else
                    console.log("\"client\" has no property or function named " + model.func);
            }
        }

        showArrow: model.showArrow
    }

    model: ListModel {
        ListElement {
            text: "Registration"
            url: "UserRegistration.qml"
            group: "pages"
            showArrow: true
        }

        ListElement {
            text: "Scheduler"
            url: "SchedulerPage.qml"
            group: "pages"
            showArrow: true
        }

        ListElement {
            text: "Reports"
            url: "ReportsPage.qml"
            group: "pages"
            showArrow: true
        }

        ListElement {
            text: "Internet is not working"
            url: "NoInternet.qml"
            group: "pages"
            showArrow: true
        }

        ListElement {
            text: "Internet is slow"
            url: "Slow.qml"
            group: "pages"
            showArrow: true
        }

        ListElement {
            text: "Btc"
            func: "btc"
            group: "test"
        }

        ListElement {
            text: "Upnp"
            func: "upnp"
            group: "test"
        }

        ListElement {
            text: "Ping"
            func: "ping"
            group: "test"
        }

        ListElement {
            text: "Event log"
            url: "EventLogPage.qml"
            group: "utils"
            showArrow: true
        }
    }
}
