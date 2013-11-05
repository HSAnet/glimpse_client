import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

ProgressBarStyle {
    panel: Rectangle {
        implicitHeight: units.gu(15)
        implicitWidth: units.gu(400)
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
