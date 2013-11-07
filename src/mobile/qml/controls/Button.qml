import QtQuick 2.0

Text {
    id: root

    signal clicked

    font.family: "Roboto Light"
    font.pixelSize: units.gu(45)
    color: "#ff3b30"

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        enabled: root.enabled
        onClicked: root.clicked()
    }

    transitions: Transition {
        NumberAnimation {
            properties: "scale,opacity"
            duration: 50
        }
    }

    states: State {
        name: "pressed"
        when: mouseArea.pressed

        PropertyChanges {
            target: root
            //scale: 0.9
            opacity: 0.6
        }
    }
}
