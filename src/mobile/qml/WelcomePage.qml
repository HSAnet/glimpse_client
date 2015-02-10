import QtQuick 2.0
import "controls"

FlickablePage {
    id: root

    readonly property bool isWizard: true

    title: qsTr("Welcome to GLIMPSE")

    contentHeight: column.height

    Column {
        id: column

        anchors {
            left: parent.left
            right: parent.right
            margins: units.gu(40)
        }

        Item { width: 1; height: units.gu(50) }

        Label {
            width: parent.width
            wrapMode: Text.Wrap
            font.pixelSize: units.gu(35)
            text: "<img src=\"images/logo.png\"/> <br/>
Thank you for installing the app. You can use <i>GLIMPSE</i> anonymously or as a registered user:<br/><br/>
<b>1)</b> As a <b>registered user</b> you can view the measurement results of all your <i>GLIMPSE</i> installations
through a web-based dashboard at www.measure-it.net.<br/>
<b>2)</b> As an <b>anonymous user</b> you won't get a dashboard, but other than that, the app works exactly the
same way.<br/>
For more information on the app, please visit us at www.measure-it.net<br/><br/>
Thanks again for installing GLIMPSE!"
        }

        Item { width: 1; height: units.gu(50) }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Continue >")
            onClicked: nextPage("WelcomeLoginPage.qml")
        }

        Item { width: 1; height: units.gu(50) }
    }
}
