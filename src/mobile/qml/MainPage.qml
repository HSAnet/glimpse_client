import QtQuick 2.0
import "controls"

Page {
    id: root

    title: "glimpse."
    /*actionTitle: {
        if (tab.currentItem && tab.currentItem.actionTitle)
            return tab.currentItem.actionTitle;

        return "settings";
    }*/

    /*function actionClicked() {
        if (tab.currentItem && tab.currentItem.actionTitle)
            return tab.currentItem.actionClicked();

        if (app.state == "")
            app.state = "settings";
        else
            app.state = "";
    }*/

    TabView {
        id: tab
        anchors.fill: parent

        onCurrentItemChanged: {
            root.analyticsTitle = Qt.binding(function() { return currentItem.title; });
        }

        Item {
            property string title: "Home"

            HomePage {
                anchors.fill: parent
            }
        }

        Item {
            property string title: "Reports"
            /*property alias actionTitle: reports.actionTitle

            function actionClicked() {
                return reports.actionClicked();
            }*/

            Reports {
                id: reports
                anchors.fill: parent
            }
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

        Item {
            property string title: "Schedules"

            Scheduler {
                anchors.fill: parent
            }
        }

//        Item {
//            property string title: "Campaigns"
//        }

        /*Item {
            property string title: "Hauptmenü"

            MenuPage {
                anchors.fill: parent
            }
        }*/
    }
}
