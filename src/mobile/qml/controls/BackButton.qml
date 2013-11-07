import QtQuick 2.1
import mplane 1.0

Item {
    id: root

    signal clicked

    property string text
    property color color: "#ff3b30"
    property real fontSize: units.gu(45)
    property bool arrowVisible: true

    width: row.width

    Row {
        id: row

        height: parent.height
        //spacing: units.gu(10)

        Item {
            id: arrowContainer

            Text {
                id: arrow
                text: "< "
                color: root.color
                font.pixelSize: root.fontSize
                anchors.verticalCenter: parent.verticalCenter
            }

            height: parent.height
            width: opacity ? arrow.width : 0
            opacity: root.arrowVisible ? 1 : 0

            Behavior on width {
                NumberAnimation {
                    easing.type: Easing.OutCubic
                }
            }

            Behavior on opacity {
                NumberAnimation {
                }
            }
        }

        Button {
            id: button
            text: root.text
            color: root.color
            font.pixelSize: root.fontSize
            anchors.verticalCenter: parent.verticalCenter
        }
    }

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
