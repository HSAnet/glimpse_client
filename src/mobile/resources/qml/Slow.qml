import QtQuick 2.0
import mplane 1.0
import "android"

ListView {
    property string title: "Internet is slow"
    property string subtitle: "Select the slow app ..."

    anchors.margins: 20
    spacing: 10

    model: ProcessModel {
    }

    delegate: Row {
        Image {
            source: "image://android/" + model.packageName
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            color: "white"
            font.pixelSize: 45
            text: model.displayName
        }
    }
}
