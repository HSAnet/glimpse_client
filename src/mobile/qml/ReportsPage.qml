import QtQuick 2.0
import QtQuick.Controls 1.0
import mplane 1.0
import "android"

Item {
    width: 100
    height: 62

    property string title: qsTr("Reports")
    property string subtitle: qsTr("see what happened recently")

    ListView {
        anchors.fill: parent

        model: ReportModel {
            scheduler: client.reportScheduler
        }

        delegate: AndroidDelegate {
            text: qsTr("%1 on %2").arg(model.taskId).arg(model.dateTime)
        }
    }
}
