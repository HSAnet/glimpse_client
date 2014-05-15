import QtQuick 2.0
import "controls"

ListView {
    id: root

    property string title: "-- no title --"
    property string subtitle

    property string actionTitle
    property bool actionEnabled: true
    property bool activity

    property string emptyText: qsTr("No entries")
    readonly property int headerHeight: units.gu(100)

    anchors.margins: units.gu(25)

    section.property: "group"
    section.delegate: Item { height: headerHeight }

    header: Rectangle {
        width: parent.width
        height: flap.height
        y: 2

        Image {
            id: flap
            anchors.right: parent.right
            //y: -parent.contentY
            y: -2
            source: "images/flap_small.png"
            height: units.gu(60)
            fillMode: Image.PreserveAspectFit
        }
    }

    Rectangle {
        width: parent.width
        height: parent.height - units.gu(60)
        z: -1
        y: -parent.contentY
    }

    Label {
        text: root.emptyText
        color: "#727277"
        anchors.horizontalCenter: parent.horizontalCenter
        y: -parent.contentY + root.height/2
        visible: parent.count == 0
    }
}
