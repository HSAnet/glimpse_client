import QtQuick 2.0
import mplane 1.0 as Mplane
import "controls"

ListPage {
    id: root
    title: "Glimpse"

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

    section.delegate: Label {
        anchors {
            left: parent.left
            right: parent.right
            rightMargin: 10
        }
        height: headerHeight
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignBottom
        text: section
        color: "gray"
    }

    model: ListModel {
        /*
        ListElement {
            text: "Registration"
            url: "UserRegistration.qml"
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
        */

        ListElement {
            text: "Btc"
            func: "btc"
            group: "Measurements"
        }

        ListElement {
            text: "HTTP Download"
            func: "http"
            group: "Measurements"
        }

        ListElement {
            text: "Upnp"
            func: "upnp"
            group: "Measurements"
        }

        ListElement {
            text: "Ping"
            func: "ping"
            group: "Measurements"
        }

        ListElement {
            text: "Dnslookup"
            func: "dnslookup"
            group: "Measurements"
        }

        ListElement {
            text: "Packet Trains"
            func: "packetTrains"
            group: "Measurements"
        }

        ListElement {
            text: "Udp Ping"
            func: "udpPing"
            group: "Measurements"
        }

        ListElement {
            text: "Traceroute"
            func: "traceroute"
            group: "Measurements"
        }

        ListElement {
            text: "Measure It!"
            func: "measureIt"
            group: "Measurements"
        }

        /*
        ListElement {
            text: "Force fetch tasks"
            func: "fetchTasks"
            group: "utils"
        }
        */

        ListElement {
            text: "Scheduler"
            url: "SchedulerPage.qml"
            group: "Services"
            showArrow: true
        }

        ListElement {
            text: "Reports"
            url: "ReportsPage.qml"
            group: "Services"
            showArrow: true
        }

        ListElement {
            text: "Event log"
            url: "EventLogPage.qml"
            group: "Services"
            showArrow: true
        }
    }
}
