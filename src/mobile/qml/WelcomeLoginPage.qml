import QtQuick 2.0
import "controls"

Page {
    id: root

    title: qsTr("login")
    activity: loginPage.activity

    Rectangle {
       color: "#f8c580"

       anchors {
           top: parent.top
           bottom: parent.bottom
           left: parent.left
           right: parent.right
       }

       Image{
           id: flap
           anchors.right: parent.right
           anchors.top: parent.top
           source: "images/flap.png"
           height: units.gu(150)
           fillMode: Image.PreserveAspectFit

           MouseArea {
               anchors.fill: parent
               onClicked: {
                   if (app.state == "")
                       app.state = "settings";
                   else
                       app.state = "";
               }

           }
       }

       Label {
           id: title
           text: qsTr("glimpse.")

           font.pixelSize: units.gu(100)
           color: "white"

           anchors {
               top: flap.bottom
               horizontalCenter: parent.horizontalCenter
               topMargin: units.gu(20)
           }
       }

       Login {
           id: loginPage
           width: parent.width
           isWizard: true

           anchors {
               top: title.bottom
               topMargin: units.gu(50)
           }
       }

       Label {
           id: divider
           color: "#979696"
           text: "or"
           anchors.horizontalCenter: parent.horizontalCenter
           anchors.top: loginPage.bottom
           anchors.topMargin: units.gu(20)
       }

       BigButton {
           anchors.horizontalCenter: parent.horizontalCenter
           anchors.top: divider.bottom
           anchors.topMargin: units.gu(20)
           text: qsTr("Register")
           onClicked: nextPage("WelcomeRegistrationPage.qml")
       }

       BigButton {
           anchors.horizontalCenter: parent.horizontalCenter
           anchors.bottom: parent.bottom
           anchors.bottomMargin: units.gu(20)
           text: qsTr("Don't register")
           onClicked: client.loginController.anonymousRegistration()
       }
    }
}
