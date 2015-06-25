import QtQuick 2.0
import mplane 1.0
import QtQuick.Controls 1.3
import QtQml 2.2
import "controls"

Rectangle {
    id: root
    width: ListView.view.width
    height: units.gu(50)
    //anchors.top: placeholder.bottom

    property alias headline: headlineitem.text
    property variant columns
    property string imageSource
    property bool showArrow: true
    property bool showImage: false
    property bool showBorder: (model.index+1 != ListView.view.count)
    signal clicked
    property bool expanded: false

    property int count: ListView.view.count
    property variant section: ListView.section
    property variant nextSection: ListView.nextSection
    property variant previousSection: ListView.previousSection

    function showResult(result) {
        foo.clear();
        for (var i = 0; i < result["results"].length; i++) {
            switch (result["task_id"]) {
            case 1:
            case 4:
            case 5:
                foo.append({"result": addPing(result["results"][i])});;
                break;
            case 10:
                foo.append({"result": addTraceroute(result["results"][i])});
                break;
            case 15:
                foo.append({"result": addHttpDownload(result["results"][i])});
            }
        }

        if (expanded) {
            expanded = false;
            height = units.gu(50);
        } else {
            expanded = true;
            height += result["results"].length * 14;
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
    Rectangle {
        id: innerRectangle

        anchors.fill: parent
        anchors.leftMargin: units.gu(40)
        anchors.rightMargin: units.gu(40)

        color: mouse.pressed ? "white" : "white"

        Item {
            id: info
            visible: root.showImage
            anchors {
                top: parent.top
                topMargin: units.gu(20)
            }

            Image {
                id: image
                width: headlineitem.font.pixelSize * 1.05
                height: width

                fillMode: Image.PreserveAspectFit
                source: root.imageSource.length ? root.imageSource : ""
                visible: root.imageSource.length > 0
                smooth: true
            }
        }

        Text {
            id: headlineitem
            //text: modelData
            color: "#333333"
            font.pixelSize: units.gu(22)
            anchors {
                top: parent.top
                left: root.showImage ? info.right : parent.left
                right: root.showArrow ? arrow.left : parent.right
                leftMargin: root.showImage ? units.gu(50) : 0
                topMargin: units.gu(25)
            }
            //            elide: Text.ElideMiddle
        }

        Text {
            id: arrow
            anchors.top: parent.top
            anchors.topMargin: units.gu(20)
            anchors.right: parent.right
            anchors.rightMargin: units.gu(20)
            anchors.verticalCenter: parent.verticalCenter
            text: "..."
            color: "#c7c7cc"
            font.bold: true
            visible: root.showArrow
        }

        ListModel {
            id: foo
        }

        ListView {
            id: bar
            model: foo
            anchors {
                top: headlineitem.bottom
                bottom: parent.bottom
            }

            delegate: Text {
                text: result
            }
        }

        Rectangle {
            id: border
            height: 1
            color: "#e2e2e2"
            anchors {
                top: bar.top
                left: parent.left
                right: parent.right
                bottomMargin: units.gu(20)
            }
            visible: root.showBorder
        }

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

