import QtQuick 2.0
import mplane 1.0
import "controls"

ListPage {
    title: qsTr("results")
    subtitle: qsTr("see what happened recently")
    //actionTitle: qsTr("upload")
    emptyText: qsTr("You don't have any results")

    model: ResultModel {
        scheduler: client.resultScheduler
    }

    delegate: ResultsDelegate {
        headline: model.taskId + "\t" + model.secondColumn
        onClicked: showResult(ListView.view.model.get(model.index))
    }

    function getHeaders(name) {
        if (name === "Ping".toUpperCase()) {
            return "Task ID\tDestination";
        } else if (name === "Traceroute".toUpperCase()) {
            return "Task ID\tDestination";
        } else if (name === "HTTPdownload".toUpperCase()) {
            return "Task ID\tHost";
        }
        return "Task ID"
    }

    section.property: "name"
    section.delegate: Component {
        id: sectionHeading
        Rectangle {
            width: parent.width
            height: 21
            color: "lightsteelblue"

            Text {
                id: header
                text: section
                font.bold: true
                color: "#333333"
                font.pixelSize: units.gu(30)
            }

            Text {
                text: getHeaders(section)
                anchors {
                    left: parent.left
                    leftMargin: units.gu(40)
                    rightMargin: units.gu(40)
                    top: header.bottom
                }
                font.pixelSize: units.gu(25)
            }
        }
    }
}
