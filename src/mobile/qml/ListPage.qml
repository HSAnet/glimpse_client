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

    section.property: "group"
    section.delegate: Item { height: headerHeight }

    Label {
        text: root.emptyText
        color: "#727277"
        anchors.centerIn: parent
        visible: parent.count == 0
    }
}
