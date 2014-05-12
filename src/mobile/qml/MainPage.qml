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

            HomePage {}
        }

        Item {
            property string title: "Toolbox"

            ToolBoxPage {
                anchors.fill: parent
            }
        }

//        Item {
//            property string title: "History"
//        }

//        Item {
//            property string title: "Campaigns"
//        }

//        Item {
//            property string title: "Hauptmenü"

//            MenuPage {
//                anchors.fill: parent
//            }
//        }
    }
}
