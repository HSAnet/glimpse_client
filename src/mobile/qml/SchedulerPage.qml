import QtQuick 2.0
import mplane 1.0
import "android"
import "controls"

ListPage {
    title: qsTr("Schedule")
    subtitle: qsTr("see what's coming next")

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
