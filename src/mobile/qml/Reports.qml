import QtQuick 2.0
import mplane 1.0
import "controls"

Page {
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

    Rectangle {
        id: listBackground
        width: app.width - units.gu(50)
        height: parent.height

        anchors {
            top: parent.top
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
            topMargin: units.gu(20)
            bottomMargin: units.gu(20)
        }

        Image {
            id: flap
            anchors.right: parent.right
            anchors.top: parent.top
            source: "images/flap_small.png"
            height: units.gu(60)
            fillMode: Image.PreserveAspectFit
        }

        ListPage {
            title: qsTr("reports")
            subtitle: qsTr("see what happened recently")
            actionTitle: qsTr("upload")
            activity: client.reportController.status == ReportController.Running
            emptyText: qsTr("You don't have any reports")

            anchors {
                fill: parent
                left: parent.left
            }



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

            model: ReportModel {
                scheduler: client.reportScheduler
            }

            delegate: ListDelegate {
                showArrow: true
                showImage: false
                showBorder: true

                headline: model.dateTime
                text: "TaskID: " + model.taskId
                onClicked: showReport(ListView.view.model.get(model.index))
            }
        }
    }
}
