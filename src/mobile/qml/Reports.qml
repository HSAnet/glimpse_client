import QtQuick 2.0
import mplane 1.0
import "controls"

ListPage {
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

    title: qsTr("reports")
    subtitle: qsTr("see what happened recently")
    actionTitle: qsTr("upload")
    activity: client.reportController.status == ReportController.Running
    emptyText: qsTr("You don't have any reports")

    /*
    header: Item {
        width: parent.width
        height: parent.headerHeight

        Label {
            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            wrapMode: Text.Wrap
            text: visible ? client.reportController.errorString() : ""
            color: "red"
            visible: client.reportController.status == ReportController.Error
        }
    }
    */

    model: ReportModel {
        scheduler: client.reportScheduler
    }

    delegate: ListDelegate {
        showArrow: true
        showImage: false

        headline: model.dateTime
        text: "TaskID: " + model.taskId
        onClicked: showReport(ListView.view.model.get(model.index))
    }
}
