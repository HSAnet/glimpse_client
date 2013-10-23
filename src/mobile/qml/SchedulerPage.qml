import QtQuick 2.0
import QtQuick.Controls 1.0
import mplane 1.0
import "android"

Item {
    width: 100
    height: 62

    property string title: qsTr("Schedule")
    property string subtitle: qsTr("see what's coming next")

    ListView {
        anchors.fill: parent

        model: SchedulerModel {
            scheduler: client.scheduler
        }

        delegate: AndroidDelegate {
            showArrow: false
            text: model.name
        }
    }
}
