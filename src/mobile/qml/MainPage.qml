import QtQuick 2.0
import "controls"

Page {
    title: "glimpse."
    actionTitle: "settings"

    function actionClicked() {
        if (app.state == "")
            app.state = "settings";
        else
            app.state = "";
    }

    TabView {
        id: tab
        anchors.fill: parent

        Item {
            property string title: "Home"

            width: tab.contentWidth
            height: tab.contentHeight

            HomePage {}
        }

        Item {
            property string title: "History"

            width: tab.contentWidth
            height: tab.contentHeight
        }

        Item {
            property string title: "Campaigns"

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
