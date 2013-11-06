import QtQuick 2.0
import mplane 1.0
import "android"
import "controls"

Item {
    width: 100
    height: 62

    property string title: qsTr("Schedule")
    property string subtitle: qsTr("see what's coming next")

    ListView {
        anchors.fill: parent

        Label {
            text: qsTr("Your schedule is empty")
            color: "lightgray"
            anchors.centerIn: parent
            visible: parent.count == 0
        }

        model: SchedulerModel {
            scheduler: client.scheduler
        }

        delegate: AndroidDelegate {
            showArrow: false
            text: model.name
        }
    }
}
