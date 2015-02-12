import QtQuick 2.0
import mplane 1.0
import "controls"

ListPage {
    anchors {
        fill: parent
        left: parent.left
    }
    title: qsTr("Schedule")
    subtitle: qsTr("see what's coming next")
    emptyText: qsTr("Your schedule is empty")

    model: SchedulerModel {
        scheduler: client.scheduler
    }

    delegate: ListDelegate {
        showArrow: false
        showImage: true
        showBorder: true

        headline: model.name
        text: qsTr("NextRun: ") + model.nextRun
        imageSource: "../images/info.png"
    }
}


