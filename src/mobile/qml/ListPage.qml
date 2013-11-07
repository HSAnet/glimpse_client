import QtQuick 2.0
import "controls"

ListView {
    id: root

    property string title: "-- no title --"
    property string subtitle

    property string actionTitle
    property bool actionEnabled: true
    property bool activity

    readonly property int headerHeight: units.gu(100)

    section.property: "group"
    section.delegate: Item { height: headerHeight }
}
