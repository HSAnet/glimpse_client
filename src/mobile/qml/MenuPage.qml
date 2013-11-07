import QtQuick 2.0
import mplane 1.0
import "android"
import "controls"

ListPage {
    title: "Glimpse"
    actionTitle: qsTr("Settings")

    function actionClicked() {
        nextPage("Settings.qml");
    }

    delegate: AndroidDelegate {
        text: model.text
        onClicked: {
            if (model.url)
                nextPage(model.url);
            else {
                var func = client[model.func];
                func();
            }
        }

        showArrow: false
    }

    model: ListModel {
        ListElement {
            text: "Registration"
            url: "UserRegistration.qml"
            group: "pages"
        }

        ListElement {
            text: "Scheduler"
            url: "SchedulerPage.qml"
            group: "pages"
        }

        ListElement {
            text: "Reports"
            url: "ReportsPage.qml"
            group: "pages"
        }

        ListElement {
            text: "Internet is not working"
            url: "Settings.qml"
            group: "pages"
        }

        ListElement {
            text: "Internet is slow"
            url: "Slow.qml"
            group: "pages"
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
    }
}
