import QtQuick 2.0

Item {
    property string title: "-- no title --"
    property string subtitle

    property string actionTitle
    property bool actionEnabled: true
    property bool activity

//    property bool spacing: true
//    readonly property int spacingValue: units.gu(25)

    y: spacing ? spacingValue : 0
    width: parent.width
//    height: parent.height-spacingValue
    height: parent.height
}
