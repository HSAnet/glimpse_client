import QtQuick 2.0

Item {
    id: root
    width: ListView.view.width
    height: units.gu(110)
    //anchors.top: placeholder.bottom

    property alias headline: headlineitem.text
    property alias text: textitem.text
    property string imageSource
    property bool showArrow: true
    property bool showImage: false
    property bool showBorder: false
    signal clicked

    property int count: ListView.view.count
    property variant section: ListView.section
    property variant nextSection: ListView.nextSection
    property variant previousSection: ListView.previousSection



    Rectangle {
        anchors.fill: parent
        anchors.leftMargin: units.gu(40)
        anchors.rightMargin: units.gu(40)

        color: mouse.pressed ? "white" : "white"

        Item {
            id: info
            visible: root.showImage
            anchors {
                top: parent.top
                topMargin: units.gu(20)
            }

            Image {
                id: image
                width: headlineitem.font.pixelSize * 1.05
                height: width

                fillMode: Image.PreserveAspectFit
                source: root.imageSource.length ? root.imageSource : ""
                visible: root.imageSource.length > 0
                smooth: true
            }
        }

        Text {
            id: headlineitem
            //text: modelData
            color: "#333333"
            font.pixelSize: units.gu(30)
            anchors {
                top: parent.top
                left: root.showImage ? info.right : parent.left
                right: root.showArrow ? arrow.left : parent.right
                leftMargin: root.showImage ? units.gu(50) : 0
                topMargin: units.gu(10)
            }
//            elide: Text.ElideMiddle
        }

        Text {
            id: textitem
            anchors {
                top: headlineitem.bottom
                topMargin: units.gu(10)
                leftMargin: root.showImage ? units.gu(50) : 0
                left: root.showImage ? info.right : parent.left
                right: root.showArrow ? arrow.left : parent.right
            }

            color: "#333333"
            font.pixelSize: units.gu(20)
            //text: modelData
//            elide: Text.ElideMiddle
        }

        Text {
            id: arrow
            anchors.right: parent.right
            anchors.rightMargin: units.gu(20)
            anchors.verticalCenter: parent.verticalCenter
            text: ">"
            color: "#c7c7cc"
            font.pixelSize: textitem.font.pixelSize * 1.2
            font.bold: true
            visible: root.showArrow
        }

        Rectangle {
            id: border
            height: 1
            color: "#e2e2e2"
            anchors {
                top: textitem.bottom
                left: parent.left
                right: parent.right
                topMargin: units.gu(20)
                bottomMargin: units.gu(20)
            }
            visible: root.showBorder
        }
    }

    MouseArea {
        id: mouse
        anchors.fill: parent
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
        when: mouse.pressed

        PropertyChanges {
            target: root
            scale: 0.95
            opacity: 0.7
        }
    }
}
