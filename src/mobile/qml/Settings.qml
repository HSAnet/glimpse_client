import QtQuick 2.0
import mplane 1.0
import "controls"

ListPage {
    property string title: qsTr("Settings")
    property string actionTitle: qsTr("About")

    function actionClicked() {
        nextPage("About.qml");
    }

    Component.onCompleted: {
        listModel.append( {"group":"general", "text":qsTr("Account"), "url":"settings/Account.qml"} );

        listModel.append( {"group":"measurements", "text":qsTr("Connection"), "url":"settings/Connection.qml"} );
        listModel.append( {"group":"measurements", "text":qsTr("Campaigns"), "url":"settings/Campaigns.qml"} );
    }

    delegate: ListDelegate {
        text: model.text
        onClicked: {
            if (model.url)
                nextPage(model.url);
        }
    }

    model: ListModel {
        id: listModel
    }
}
