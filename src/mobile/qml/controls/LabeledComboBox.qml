import QtQuick 2.0

Row {
    width: parent.width
    spacing: 10

    property double controlScale: 0.7
    property alias label: controlsLabel.text
    property alias currentIndex: controlsComboBox.currentIndex
    property alias model: controlsComboBox.model

    Label {
        id: controlsLabel
        width: (parent.width - 5) * (1 - controlScale)
        anchors.verticalCenter: parent.verticalCenter
    }
    ComboBox {
        id: controlsComboBox
        width: (parent.width - 5) * controlScale
    }
}
