import QtQuick 2.0
import mplane 1.0 as Mplane
import "controls"

Page{
    Rectangle {
        id: notificationPanel
        color: "#becbcb"
        width: app.width - 50
        height: 100

        anchors {
            horizontalCenter: parent.horizontalCenter
        }
//        ListPage {

//        }
    }

    Rectangle {
        id: historyPanel
        color: "white"
        width: app.width
        height: 180

        anchors {
            top: notificationPanel.bottom
            topMargin: 20
        }

        Text {
            id: historyTitle
            text: qsTr("History")
            font {
                weight: Font.Normal
                family: "Helvetica Neue"
                pixelSize: units.gu(35)
            }
        }
    }

    Rectangle {
        id: buttonPanel
        width: app.width
        height: 100
        color: "#f8c580"

        anchors {
            top: historyPanel.bottom
            topMargin: 18
        }

        Button {
            id: button1
            width: app.width / 3
            height: parent.height

            Text {
                text: qsTr("MEASURE IT")
                color: "white"
                width: parent.width
                height: parent.height
                font {
                    weight: Font.Bold
                    family: "Helvetica Neue"
                    pixelSize: units.gu(25)
                }
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        Button {
            id: button2
            width: app.width / 3
            height: parent.height

            anchors {
                left: button1.right
            }

            Text {
                text: qsTr("MY NET IS NOT WORKING")
                color: "white"
                width: parent.width
                height: parent.height
                font {
                    weight: Font.Bold
                    family: "Helvetica Neue"
                    pixelSize: units.gu(25)
                }
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        Button {
            id: button3
            width: app.width / 3
            height: parent.height

            anchors {
                left: button2.right
            }

            Text {
                text: qsTr("MY NET IS POOR")
                color: "white"
                width: parent.width
                height: parent.height
                font {
                    weight: Font.Bold
                    family: "Helvetica Neue"
                    pixelSize: units.gu(25)
                }
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }

    Rectangle {
        id: campaignsPanel
        color: "white"
        width: app.width
        height: 100

        anchors {
            top: buttonPanel.bottom
            topMargin: 20
        }

        Text {
            id: campaignsTitle
            text: qsTr("Campaigns")
            font {
                weight: Font.Normal
                family: "Helvetica Neue"
                pixelSize: units.gu(35)
            }
        }
    }
}
