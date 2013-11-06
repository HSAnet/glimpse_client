import QtQuick 2.0

Flickable {
    property string title: "-- no title --"
    property string subtitle

    property string actionTitle
    property bool actionEnabled: true
    property bool activity

    width: parent.width
    height: parent.height

    flickableDirection: Flickable.VerticalFlick
    contentHeight: childrenRect.height
}
