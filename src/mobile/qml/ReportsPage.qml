import QtQuick 2.0
import QtQuick.Controls 1.0
import mplane 1.0
import "android"

Item {
    id: root

    width: 100
    height: 62

    property string title: qsTr("Reports")
    property string subtitle: qsTr("see what happened recently")
    property string actionTitle: qsTr("Upload")

    function actionClicked() {
        client.reportController.sendReports();
    }

    function showReport(report) {
        var params = {
            item: Qt.resolvedUrl("ResultPage.qml"),
            properties: {
                subtitle: qsTr("report details"),
                resultText: JSON.stringify(report, null, "   ")
            }
        }

        pageStack.push(params);
    }

    ListView {
        anchors.fill: parent

        model: ReportModel {
            scheduler: client.reportScheduler
        }

        delegate: AndroidDelegate {
            text: qsTr("%1 on %2").arg(model.taskId).arg(model.dateTime)
            onClicked: showReport(ListView.view.model.get(model.index))
        }
    }
}
