import QtQuick 2.0
import "controls"

Item {
    id: root

    property string title: qsTr("About")

    Column {
        anchors.centerIn: parent
        spacing: units.gu(20)

        Image {
            anchors.horizontalCenter: parent.horizontalCenter
            source: "images/hsa-logo.png"
        }

        Column {
            anchors.horizontalCenter: parent.horizontalCenter

            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Hochschule Augsburg")
            }
            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("University of Applied Science")
            }
            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Copyright (c) 2012-2013")
            }
        }

        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("measure-it.net")
        }
    }
}
