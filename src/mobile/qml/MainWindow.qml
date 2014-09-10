import QtQuick 2.2
import QtQuick.Controls 1.0
import mplane 1.0

ApplicationWindow {
    width: units.gu(768)
    height: units.gu(1200)
    visible: true

    Loader {
        id: loader
        anchors.fill: parent
        source: "main.qml"
    }
}
