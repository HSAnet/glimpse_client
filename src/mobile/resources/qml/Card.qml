import QtQuick 2.0
import QtQuick.Layouts 1.0
import mplane 1.0

BorderImage {
    id: root

    property string title: "title"
    property bool expanded: true

    default property alias contents: contentArea.children

    source: "images/card_background.png"
    horizontalTileMode: BorderImage.Repeat
    verticalTileMode: BorderImage.Repeat
    border {
        left: 15
        right: 15
        top: 15
        bottom: 15
    }

    property int minHeight: titleText.height + 2*layout.anchors.margins

    height: expanded ? Math.max(layout.height, minHeight) : minHeight

    Behavior on height {
        NumberAnimation {
            easing.overshoot: 0.952
            easing.type: Easing.OutBack
            duration: 150

        }
    }

    Item {
        ParallelAnimation {
            id: moveAnimation

            property real v

            NumberAnimation {
                id: moveAnimationCard

                target: root
                property: "x"
                to: v > 0 ? root.width : -root.width
                duration: 150
            }

            ScriptAction {
                script: {
                    root.visible = false;
                    root.height = 0;
                }
            }
        }
    }

    MouseArea {
        property Time timer: Time {}

        anchors.fill: parent
        drag.target: root
        drag.axis: Drag.XAxis
        onPressed: timer.restart()
        onReleased: {
            var v = root.x*1000 / timer.elapsed();
            if ( Math.abs(v) > 500 ) {
                moveAnimation.v = v;
                moveAnimation.start();
            } else {
                root.x = 0;
            }
        }
    }

    ColumnLayout {
        id: layout
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            margins: 10
        }
        //spacing: 10

        Text {
            id: titleText
            Layout.fillWidth: true
            text: root.title
            font.family: "Roboto Light"
            color: "#707070"
            font.pixelSize: 50

            Image {
                source: !root.expanded ? "images/minimize.png" : "images/maximize.png"
                anchors {
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: root.expanded = !root.expanded
                }
            }
        }

        Item {
            id: contentArea
            anchors {
                left: parent.left
                right: parent.right
            }

            clip: true
            height: childrenRect.height
        }
    }
}
