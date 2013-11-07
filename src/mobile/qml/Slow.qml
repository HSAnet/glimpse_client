import QtQuick 2.0
import mplane 1.0
import "android"
import "controls"

ListPage {
    title: qsTr("Internet is slow")
    actionTitle: qsTr("Skip")

    function actionClicked() {
    }

    header: Label {
        text: qsTr("Which app is slow?")
        width: parent.width
        height: units.gu(100)
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    model: ProcessModel {}

    delegate: AndroidDelegate {
        text: model.displayName
        imageSource: "image://android/" + model.packageName
    }
}
