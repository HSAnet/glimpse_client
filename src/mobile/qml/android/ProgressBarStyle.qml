import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

ProgressBarStyle {
    panel: Rectangle {
        implicitHeight: 15
        implicitWidth: 400
        color: "#444"
        opacity: 0.8
        Rectangle {
            antialiasing: true
            radius: 1
            color: "#468bb7"
            height: parent.height
            width: parent.width * control.value / control.maximumValue
        }
    }
}