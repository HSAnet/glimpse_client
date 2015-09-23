import QtQuick 2.0

Row {
    width: parent.width
    spacing: 10

    property alias label: controlsLabel.text
    property alias text: controlsTextField.text
    property double controlsScale: 0.7

    Label {
        id: controlsLabel
        width: (parent.width - 5) * (1 - controlsScale)
        anchors.verticalCenter: parent.verticalCenter
    }
    TextField {
        id: controlsTextField
        width: (parent.width - 5) * controlsScale
    }
}
