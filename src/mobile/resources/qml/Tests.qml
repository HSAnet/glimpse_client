import QtQuick 2.0
import mplane 1.0
import QtQuick.Controls 1.0
import QtQuick.Window 2.0
import "android"

Item {
    width: 400
    height: 500

    property string title: qsTr("Manual tests")
    property string subtitle: qsTr("Listing all tests")

    ListView {
        anchors.fill: parent

        model: ListModel {
            ListElement {
                title: "Internet does not work"
            }

            ListElement {
                title: "Packet train"
            }
        }

        delegate: AndroidDelegate {
            text: title
        }
    }
}
