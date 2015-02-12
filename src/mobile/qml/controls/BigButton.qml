import QtQuick 2.0

Rectangle {
    id: bigButton
    color: "#f7b35f"
    width: units.gu(550)
    height: units.gu(80)

    signal clicked
    property alias text: buttonLabel.text

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        enabled: bigButton.enabled
        onClicked: bigButton.clicked()
    }

    Text{
        id: buttonLabel
        anchors.centerIn: parent
        font.family: "Roboto Light"
        font.pixelSize: units.gu(40)
        color: "white"
        text: qsTr("login")
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
            target: bigButton
            //scale: 0.9
            opacity: 0.6
        }
    }
}
