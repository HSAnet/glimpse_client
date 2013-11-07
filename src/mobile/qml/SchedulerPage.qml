import QtQuick 2.0
import mplane 1.0
import "controls"

ListPage {
    title: qsTr("Schedule")
    subtitle: qsTr("see what's coming next")
    emptyText: qsTr("Your schedule is empty")

    model: SchedulerModel {
        scheduler: client.scheduler
    }

    delegate: ListDelegate {
        showArrow: false
        text: model.name
    }
}
