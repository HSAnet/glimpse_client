import QtQuick 2.0
import "android"

Item {
    id: root

    property string leftText
    property variant rightText
    property bool showSpinner: false
    property bool showCheckmark: false
    property bool checkmark: true

    width: ListView.view.width
    height: 50

    Text {
        anchors {
            left: parent.left
            leftMargin: 15
            verticalCenter: parent.verticalCenter
        }

        text: root.leftText
        font.pixelSize: 30
        font.family: "Roboto Light"
    }

    Item {
        anchors {
            right: checkMark.left
            rightMargin: 10
            verticalCenter: parent.verticalCenter
        }

        height: 50
        width: Math.max(70, resultText.width)

        Text {
            id: resultText
            anchors.centerIn: parent
            text: root.rightText
            font.pixelSize: 20
            font.family: "Roboto Light"
            color: "#707070"
            visible: (root.showCheckmark && root.checkmark) || !root.showCheckmark && typeof(rightText) != "boolean"
        }
    }

    Image {
        id: checkMark

        anchors {
            right: parent.right
            rightMargin: 10
            verticalCenter: parent.verticalCenter
        }

        source: root.checkmark ? "images/connectiontester_ok.png" : "images/connectiontester_fail.png"
        visible: root.showCheckmark
    }

    Spinner {
        width: 65
        anchors {
            right: parent.right
            rightMargin: 10
            verticalCenter: parent.verticalCenter
        }
        running: root.showSpinner
    }
}
