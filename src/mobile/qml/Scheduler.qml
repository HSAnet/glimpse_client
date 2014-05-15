import QtQuick 2.0
import mplane 1.0
import "controls"

Page {

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
                text: "NextRun: " + model.nextRun
                imageSource: "../images/info.png"
            }
        }
    }
}


