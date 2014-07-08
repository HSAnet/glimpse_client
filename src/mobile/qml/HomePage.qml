import QtQuick 2.0
import mplane 1.0
import "controls"
import "controls/charts/linechart.js" as Chart

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
        height: diagram.height + units.gu(100)

        anchors {
            top: notificationPanel.bottom
            topMargin: units.gu(20)
        }

        Text {
            id: historyTitle
            text: qsTr("History")
            color: "#dea65a"
            font {
                weight: Font.Normal
                family: "Helvetica Neue"
                pixelSize: units.gu(35)
            }
            anchors {
                left: parent.left
                leftMargin: units.gu(20)
                top: parent.top
                topMargin: units.gu(20)
            }
        }

        Rectangle {
            id: border2
            height: 1
            color: "#e2e2e2"
            anchors {
                top: historyTitle.bottom
                left: parent.left
                leftMargin: units.gu(20)
                right: parent.right
                rightMargin: units.gu(20)
                topMargin: units.gu(10)
            }
        }


        Canvas {
            id: diagram
            width: parent.width - units.gu(50)
            height: 220

            onPaint: {
                var chart = new Chart.LineChart();
                chart.width = width;
                chart.matrix_textPaddingLeft = 0.7;
                chart.drawLineChart(0, 0, getContext("2d"));
            }
            anchors {
                left: parent.left
                top: border2.bottom

                leftMargin: units.gu(10)
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
            text: qsTr("EventLog")
            color: "#dea65a"
            anchors {
                left: parent.left
                leftMargin: units.gu(20)
                top: parent.top
                topMargin: units.gu(20)
            }

            font {
                weight: Font.Normal
                family: "Helvetica Neue"
                pixelSize: units.gu(35)
            }
        }

        Rectangle {
            id: border
            height: 1
            color: "#e2e2e2"
            anchors {
                top: campaignsTitle.bottom
                left: parent.left
                leftMargin: units.gu(20)
                right: parent.right
                rightMargin: units.gu(20)
                topMargin: units.gu(10)
            }
        }


        ListView {
            model: logModel
            clip: true

            anchors {
                top: border.bottom
                topMargin: units.gu(20)
                bottom: parent.bottom
                bottomMargin: units.gu(40)
                left: parent.left
                right: parent.right
                leftMargin: units.gu(10)
                rightMargin: units.gu(10)
            }

            Component.onCompleted: positionViewAtEnd()
            onCountChanged: positionViewAtEnd()

            delegate: Item {
                width: parent.width
                height: textItem.height

                Rectangle {
                    id: dot
                    width: units.gu(22)
                    height: width
                    radius: height/2

                    anchors {
                        left: parent.left
                        leftMargin: units.gu(10)
                        top: parent.top
                        topMargin: units.gu(3)
                    }
                    color: {
                        switch (model.level) {
                        case 0: return "#f8c580";
                        case 1: return "#a8f880";
                        case 2: return "#d1e0e0";
                        case 3: return "white";
                        case 4: return "#f8f180";
                        case 5: return "#f88380";
                        }
                    }
                }

                Label {
                    id: textItem
                    width: parent.width
                    font.pixelSize: units.gu(25)
                    color: "#333333"
                    wrapMode: Text.Wrap
                    text: model.message
                    anchors {
                        left: dot.right
                        right: parent.right
                        leftMargin: units.gu(15)
                    }
                }
            }
        }
    }
}
