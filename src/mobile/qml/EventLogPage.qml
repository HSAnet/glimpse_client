import QtQuick 2.0
import "controls"

ListPage {
    id: root

    Component.onCompleted: {
        positionViewAtEnd()
    }

    onCountChanged: positionViewAtEnd()

    title: qsTr("Event log")
    spacing: 1
    model: logModel

    delegate: Rectangle {
        width: ListView.view.width
        height: textItem.height

        color: {
            switch (model.level) {
            case 0: return "orange";
            case 1: return "green";
            case 2: return "lightblue";
            case 3: return "white";
            case 4: return "yellow";
            case 5: return "red";
            }
        }

        Label {
            id: textItem
            width: parent.width
            wrapMode: Text.Wrap
            text: model.message
        }
    }
}
