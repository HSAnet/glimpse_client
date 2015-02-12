import QtQuick 2.0
import mplane 1.0
import "controls"

ListPage {
    title: qsTr("Internet is slow")
    actionTitle: qsTr("Skip")

    /*function actionClicked() {
    }*/

    header: Label {
        text: qsTr("Which app is slow?")
        width: parent.width
        height: units.gu(100)
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        color: "#727277"
        font.pixelSize: units.gu(35)
    }

    model: ProcessModel {}

    delegate: ListDelegate {
        text: model.displayName
        imageSource: "image://android/" + model.packageName
    }
}
