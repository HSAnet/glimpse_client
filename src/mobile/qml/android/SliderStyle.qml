import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import mplane 1.0

SliderStyle {
    handle: Rectangle {
        width: units.gu(30)
        height: units.gu(30)
        radius: height
        antialiasing: true
        color: Qt.lighter("#468bb7", 1.2)
    }

    groove: Item {
        implicitHeight: units.gu(50)
        implicitWidth: units.gu(400)
        Rectangle {
            height: units.gu(8)
            width: parent.width
            anchors.verticalCenter: parent.verticalCenter
            color: "#444"
            opacity: 0.8
            Rectangle {
                antialiasing: true
                radius: units.gu(1)
                color: "#468bb7"
                height: parent.height
                width: parent.width * control.value / control.maximumValue
            }
        }
    }
}
