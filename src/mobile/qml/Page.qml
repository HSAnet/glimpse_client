import QtQuick 2.0

Flickable {
    property string title: "-- no title --"
    property string subtitle
    property string actionTitle

    function nextPage(componentName, properties) {
        var params = {
            item: Qt.resolvedUrl(componentName),
            properties: properties
        }

        pageStack.push(params);
    }

    width: parent.width
    height: parent.height

    flickableDirection: Flickable.VerticalFlick
    contentHeight: childrenRect.height
}
