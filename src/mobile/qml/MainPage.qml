import QtQuick 2.0
import "controls"

Page {
    title: "glimpse."
    spacing: false

    TabView {
        id: tab
        anchors.fill: parent

        Rectangle {
            property string title: "Home"

            color: "red"
            width: tab.contentWidth
            height: tab.contentHeight
        }

        Rectangle {
            property string title: "History"

            color: "blue"
            width: tab.contentWidth
            height: tab.contentHeight
        }

        Rectangle {
            property string title: "Campaigns"

            color: "blue"
            width: tab.contentWidth
            height: tab.contentHeight
        }

        Item {
            property string title: "Hauptmenü"

            width: tab.contentWidth
            height: tab.contentHeight

            MenuPage {
                anchors.fill: parent
            }
        }
    }
}
