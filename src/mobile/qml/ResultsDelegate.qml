import QtQuick 2.0
import mplane 1.0
import QtQuick.Controls 1.3
import QtQml 2.2
import "controls"

Rectangle {
    id: root
    width: ListView.view.width
    height: units.gu(60)
    anchors {
        top: sectionHeading.bottom
    }

    property alias headline: headlineitem.text
    property variant columns
    signal clicked
    property bool expanded: false

    property int count: ListView.view.count
    property variant section: ListView.section
    property variant nextSection: ListView.nextSection
    property variant previousSection: ListView.previousSection

    function showResult(result) {
        resultsModel.clear();

        for (var i = 0; i < result["results"].length; i++) {
            switch (String(name).toLowerCase()) {
            case "ping":
                resultsModel.append({"result": addPing(result["results"][i])});;
                break;
            case "traceroute":
                resultsModel.append({"result": addTraceroute(result["results"][i])});
                break;
            case "httpdownload":
                resultsModel.append({"result": addHttpDownload(result["results"][i])});
                break;
            case "packettrains":
                resultsModel.append({"result": addPackettrains(result["results"][i])});
                break;
            case "btc_ma":
                resultsModel.append({"result": addBtc(result["results"][i])});
                break;
            case "dnslookup":
                resultsModel.append({"result": addDnsLookup(result["results"][i])});
                break;
            case "reversednslookup":
                resultsModel.append({"result": addReverseDnsLookup(result["results"][i])});
                break;
            case "upnp":
                resultsModel.append({"result": addUpnp(result["results"][i])});
                break;
            case "wifilookup":
                resultsModel.append({"result": addWifiLookup(result["results"][i])});
                break;
            }
        }

        if (expanded) {
            expanded = false;
            height = units.gu(60);
            innerRectangle.visible = false;
        } else {
            expanded = true;
            height += result["results"].length * 14;
            innerRectangle.visible = true;
        }
    }

    function addPing(result) {
        var out;
        out = result["end_time"].toLocaleString(Qt.locale(), "yyyy-MM-dd HH:mm:ss") + "\t";
        out += result["probe_result"]["round_trip_avg"].toFixed(2) + "ms\t";
        out += result["probe_result"]["round_trip_loss"].toFixed(2) + "% loss";
        return out
    }

    function addTraceroute(result) {
        var out;
        out = result["end_time"].toLocaleString(Qt.locale(), "yyyy-MM-dd HH:mm:ss") + "\t";
        out += result["probe_result"]["results"][0]["rtt_avg"].toFixed(2) + "ms\t";
        out += result["probe_result"]["hop_count"] + " hops";
        return out
    }

    function addHttpDownload(result) {
        var out;
        out = result["end_time"].toLocaleString(Qt.locale(), "yyyy-MM-dd HH:mm:ss") + "\t";
        out += (result["probe_result"]["bandwidth_bps_avg"] / 1024).toFixed(2) + " kB/s\t";
        out += result["probe_result"]["actual_num_threads"] + " threads";
        return out
    }

    function addPackettrains(result) {
        var out;
        out = result["end_time"].toLocaleString(Qt.locale(), "yyyy-MM-dd HH:mm:ss") + "\t";
        out += (result["probe_result"]["sending_speed"] / 1024).toFixed(2) + " kB/s\t";
        out += (result["probe_result"]["receiving_speed"] / 1024).toFixed(2) + " kB/s";
        return out
    }

    function addBtc(result) {
        var out;
        out = result["end_time"].toLocaleString(Qt.locale(), "yyyy-MM-dd HH:mm:ss") + "\t";
        out += (result["probe_result"]["kBs_avg"]).toFixed(2) + " kB/s\t";
        out += (result["probe_result"]["kBs_max"]).toFixed(2) + " kB/s max";
        return out
    }

    function addDnsLookup(result) {
        var out;
        out = result["end_time"].toLocaleString(Qt.locale(), "yyyy-MM-dd HH:mm:ss") + "\t";
        out += result["probe_result"]["name"] + "\t";
        out += result["probe_result"]["ttl"] + "ms";
        return out
    }

    function addReverseDnsLookup(result) {
        var out;
        out = result["end_time"].toLocaleString(Qt.locale(), "yyyy-MM-dd HH:mm:ss") + "\t";
        out += result["probe_result"]["address"];
        return out
    }

    function addUpnp(result) {
        var out;
        out = result["end_time"].toLocaleString(Qt.locale(), "yyyy-MM-dd HH:mm:ss") + "\t";
        out += result["probe_result"]["data"];
        return out
    }

    function addWifiLookup(result) {
        var out;
        out = result["end_time"].toLocaleString(Qt.locale(), "yyyy-MM-dd HH:mm:ss") + "\t";
        return out
    }

    Text {
        id: headlineitem
        color: "#333333"
        font.pixelSize: units.gu(22)
        anchors {
            top: parent.top
            topMargin: units.gu(5)
            left: parent.left
            leftMargin: units.gu(40)
        }

        MouseArea {
            id: mouse
            anchors.fill: parent
            onClicked: root.clicked()
        }

        transitions: Transition {
            NumberAnimation {
                properties: "scale,opacity"
                duration: 50
            }
        }

        states: State {
            name: "pressed"
            when: mouse.pressed

            PropertyChanges {
                target: innerRectangle
                scale: 0.95
                opacity: 0.7
            }
        }
    }

    Rectangle {
        id: innerRectangle
        anchors {
            top: headlineitem.bottom
            topMargin: units.gu(10)
            bottom: parent.bottom
            left: parent.left
            leftMargin: units.gu(40)
            right: parent.right
        }

        ListModel {
            id: resultsModel
        }

        ListView {
            id: results
            model: resultsModel
            height: parent.width

            delegate: Text {
                text: result
            }
        }
    }
}

