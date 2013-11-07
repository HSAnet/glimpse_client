import QtQuick 2.0

Item {
    id: root
    width: ListView.view.width
    height: units.gu(100)

    property alias text: textitem.text
    property string imageSource
    property bool showArrow: true
    signal clicked

    property int count: ListView.view.count
    property variant section: ListView.section
    property variant nextSection: ListView.nextSection
    property variant previousSection: ListView.previousSection

    Rectangle {
        anchors.fill: parent
        color: mouse.pressed ? "#d9d9d9" : "white"
    }

    Rectangle {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 1
        color: "#d1d0d5"
        visible: model.index == 0 || section != previousSection
    }

    Rectangle {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: 1
        color: "#d1d0d5"
        visible: model.index == root.count-1 || (section != nextSection && model.index)
    }

    Item {
        anchors {
            top: parent.top
            left: parent.left
            right: textitem.left
            bottom: parent.bottom
        }

        Image {
            id: image

            width: textitem.font.pixelSize * 1.5
            height: width
            anchors.centerIn: parent

            fillMode: Image.PreserveAspectFit
            source: root.imageSource.length ? root.imageSource : ""
            visible: root.imageSource.length > 0
            smooth: true
        }
    }

    Text {
        id: textitem
        color: "black"
        font.pixelSize: units.gu(45)
        text: modelData
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: units.gu(100)
        anchors.right: arrow.left
        elide: Text.ElideMiddle
    }

    Rectangle {
        anchors.left: textitem.left
        anchors.right: parent.right
        anchors.margins: units.gu(15)
        height: 1
        color: "#d1d0d5"
        visible: model.index > 0
    }

    Image {
        id: arrow
        anchors.right: parent.right
        anchors.rightMargin: units.gu(20)
        anchors.verticalCenter: parent.verticalCenter
        source: "images/navigation_next_item.png"
        visible: root.showArrow
    }

    MouseArea {
        id: mouse
        anchors.fill: parent
        onClicked: root.clicked()
    }

    /*
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
            target: textitem
            scale: 0.9
            opacity: 0.6
        }
    }
    */
}
