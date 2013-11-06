import QtQuick 2.0
import mplane 1.0
import "android"
import "controls"

ListView {
    property string title: qsTr("Internet is slow")
    property string subtitle
    property string actionTitle: qsTr("Skip")

    function actionClicked() {
    }

    anchors.margins: units.gu(20)
    spacing: units.gu(10)

    header: Label {
        text: "Which app is slow?"
    }

    model: ProcessModel {
    }

    delegate: Item {
        width: ListView.view.width
        height: item.height

        Row {
            id: item

            Image {
                source: "image://android/" + model.packageName
            }

            Label {
                anchors.verticalCenter: parent.verticalCenter
                text: model.displayName
            }
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            enabled: root.enabled
            //onClicked: root.clicked()
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
                target: item
                scale: 0.9
                opacity: 0.6
            }
        }
    }
}
