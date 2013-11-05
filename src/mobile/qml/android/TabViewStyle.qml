import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

TabViewStyle {
    tabsAlignment: Qt.AlignVCenter
    tabOverlap: 0
    frame: Item { }
    tab: Item {
        implicitWidth: control.width/control.count
        implicitHeight: 50
        BorderImage {
            anchors.fill: parent
            border.bottom: 8
            border.top: 8
            source: tab.selected ? "images/tab_selected.png":"images/tabs_standard.png"
            Text {
                anchors.centerIn: parent
                color: "white"
                text: tab.title.toUpperCase()
                font.pixelSize: units.gu(16)
            }
            Rectangle {
                visible: index > 0
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.margins: units.gu(10)
                width:units.gu(1)
                color: "#3a3a3a"
            }
        }
    }
}
