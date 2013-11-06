import QtQuick 2.0

Flickable {
    property string title: "-- no title --"
    property string subtitle
    property string actionTitle

    width: parent.width
    height: parent.height

    flickableDirection: Flickable.VerticalFlick
    contentHeight: childrenRect.height
}
