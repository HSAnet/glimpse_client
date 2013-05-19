import QtQuick 2.0

Item {
    id: root
    width: 76
    height: width
    opacity: running ? 1.0 : 0.0

    property bool running: false

    Behavior on opacity {
        NumberAnimation {
            duration: 250
        }
    }

    Image {
        id: image
        anchors.fill: parent
        smooth: true
        source: "images/spinner_black_76.png"
        transformOrigin: Item.Center
    }

    RotationAnimation {
        target: image
        duration: 500
        direction: RotationAnimation.Clockwise
        running: root.running
        property: "rotation"
        from: 0.0
        to: 360.0
        loops: -1
    }
}
