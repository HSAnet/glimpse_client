import QtQuick 2.0
import mplane 1.0 as Mplane
import "controls"

Page{
    title: "Home"

    Rectangle {
        color: "white"
        width: app.width
        height: 100

        Label {
            id: label
            text: qsTr("History")
        }
    }
}
