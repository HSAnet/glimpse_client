import QtQuick 2.0
import mplane 1.0
import "controls"

Item {
    property string title: qsTr("Settings")
    property string actionTitle: qsTr("About")

    function actionClicked() {
        nextPage("About.qml");
    }

    Label {
        text: qsTr("This page is under construction")
        color: "lightgray"
        anchors.centerIn: parent
    }
}
