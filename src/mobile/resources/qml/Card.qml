import QtQuick 2.0
import QtQuick.Layouts 1.0

BorderImage {
    id: root

    property string title: "title"
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

    height: layout.height

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
            Layout.fillWidth: true
            text: root.title
            font.family: "Roboto Light"
            color: "#707070"
            font.pixelSize: 50

            Image {
                source: "images/maximize.png"
                anchors {
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                }
            }
        }

        Item {
            id: contentArea
            anchors {
                left: parent.left
                right: parent.right
            }

            height: childrenRect.height
        }
    }
}
