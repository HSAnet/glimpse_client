import QtQuick 2.0
import mplane 1.0
import "controls"

ListPage {
    title: qsTr("results")
    subtitle: qsTr("see what happened recently")
    emptyText: qsTr("You don't have any results")

    model: ResultModel {
        scheduler: client.resultScheduler
    }

    delegate: ResultsDelegate {
        headline: getIdText(model.taskId) + "\t" + model.secondColumn
        onClicked: showResult(ListView.view.model.get(model.index))
        showBorder: false
        showArrow: false
    }

    function getIdText(id) {
        if (id < -1) {
            return "toolbox";
        } else {
            return id;
        }
    }

    function getColumns(name) {
        switch (String(name).toLowerCase()) {
        case "ping":
        case "traceroute":
        case "packettrains":
            return "Task ID\tDestination";
        case "httpdownload":
        case "btc_ma":
        case "dnslookup":
        case "reversednslookup":
        case "upnp":
        case "wifilookup":
            return "Task ID\tHost";
        }

        return "Task ID";
    }

    function getTitle(name) {
        switch (String(name).toLowerCase()) {
        case "ping":
            return "Ping";
        case "traceroute":
            return "Traceroute"
        case "httpdownload":
            return "HTTP Download";
        case "btc_ma":
            return "Bulk Transport Capacity"
        case "dnslookup":
            return "DNS Lookup";
        case "packettrains":
            return "Packet Trains";
        case "reversednslookup":
            return "Reverse DNS Lookup";
        case "upnp":
            return "UPnP";
        case "wifilookup":
            return "WiFi Lookup";
        }

        return name;
    }

    section.property: "name"
    section.delegate: Component {
        id: sectionHeading
        Rectangle {
            width: parent.width
            height: 21
            color: "lightsteelblue"

            Text {
                id: header
                text: getTitle(section)
                font.bold: true
                color: "#333333"
                font.pixelSize: units.gu(30)
                anchors.left: parent.left
                anchors.leftMargin: units.gu(10)
            }

            Text {
                text: getColumns(section)
                anchors {
                    left: parent.left
                    leftMargin: units.gu(40)
                    rightMargin: units.gu(40)
                    top: header.bottom
                    topMargin: units.gu(10)
                }
                font.pixelSize: units.gu(25)
            }
        }
    }
}
