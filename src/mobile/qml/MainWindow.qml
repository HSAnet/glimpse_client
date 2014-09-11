import QtQuick 2.2
import QtQuick.Controls 1.0
import mplane 1.0

/**
  * This is the real application window
  */
ApplicationWindow {
    width: units.gu(768)
    height: units.gu(1200)
    visible: true

    // Forward the back button to the loader item
    Item {
        focus: true
        Keys.onReleased: {
            loader.item.Keys.onReleased(event)
        }
    }

    // Load main.qml as our content item
    Loader {
        id: loader
        anchors.fill: parent
        source: "main.qml"
    }
}
