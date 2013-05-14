import QtQuick 2.0
import mplane 1.0
import QtQuick.Controls 1.0
import QtQuick.Window 2.0
import "android"

Item {
    width: 400
    height: 500

    property string title: qsTr("Settings")
    property string subtitle: qsTr("all you want to change")

    ListView {
        anchors.fill: parent

        model: ListModel {
            ListElement {
                title: "Profile"
            }

            ListElement {
                title: "Network"
            }
        }

        delegate: AndroidDelegate {
            text: title
        }
    }
}
