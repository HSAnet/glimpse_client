import QtQuick 2.0
import mplane 1.0
import "controls"

FlickablePage {
    contentHeight: campaignsPanel.y + campaignsPanel.height

    Canvas {
        x: notificationPanel.width - units.gu(100)
        width: units.gu(40)
        height: units.gu(20)
        anchors.bottom: notificationPanel.top

        onPaint: {
            var ctx = getContext("2d");

            ctx.fillStyle = notificationPanel.color;
            ctx.moveTo(0, height);
            ctx.lineTo(width/2, 0);
            ctx.lineTo(width, height);
            ctx.closePath();
            ctx.fill();
        }
    }

    Rectangle {
        id: notificationPanel
        color: "#becbcb"
        width: app.width - units.gu(50)
        height: units.gu(180)

        anchors {
            top: parent.top
            horizontalCenter: parent.horizontalCenter
            topMargin: units.gu(20)
        }
//        ListPage {

//        }
    }

    Rectangle {
        id: historyPanel
        color: "white"
        width: app.width
        height: units.gu(400)

        anchors {
            top: notificationPanel.bottom
            topMargin: units.gu(20)
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
        height: units.gu(180)
        color: "#f8c580"

        anchors {
            top: historyPanel.bottom
            topMargin: units.gu(18)
        }

        Button {
            id: button1
            width: app.width / 3
            height: parent.height

            Text {
                text: qsTr("SPEEDTEST")
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
        height: units.gu(300)

        anchors {
            top: buttonPanel.bottom
            topMargin: units.gu(20)
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
