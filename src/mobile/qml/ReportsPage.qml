import QtQuick 2.0
import mplane 1.0
import "android"
import "controls"

Item {
    id: root

    width: 100
    height: 62

    property string title: qsTr("Reports")
    property string subtitle: qsTr("see what happened recently")
    property string actionTitle: qsTr("Upload")
    property bool activity: client.reportController.status == ReportController.Running

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

        Label {
            text: qsTr("You don't have any reports")
            color: "lightgray"
            anchors.centerIn: parent
            visible: parent.count == 0
        }

        header: Label {
            width: parent.width
            wrapMode: Text.Wrap
            text: visible ? client.reportController.errorString() : ""
            height: visible ? implicitHeight : 0
            color: "red"
            visible: client.reportController.status == ReportController.Error

            Behavior on height {
                NumberAnimation {
                    easing.type: Easing.OutQuad
                }
            }
        }

        model: ReportModel {
            scheduler: client.reportScheduler
        }

        delegate: AndroidDelegate {
            text: model.taskId
            onClicked: showReport(ListView.view.model.get(model.index))
        }
    }
}
