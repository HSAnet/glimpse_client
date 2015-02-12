import QtQuick 2.0
import mplane 1.0
import "controls"

Page {
    id: root

    title: qsTr("Registration")

    /*function registerAnonymous() {
        client.loginController.anonymousRegistration();
    }*/

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

           /*MouseArea {
               anchors.fill: parent
               onClicked: {
                   if (app.state == "")
                       app.state = "settings";
                   else
                       app.state = "";
               }
           }*/
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
           loginMode: false
           buttonTitle: qsTr("Register")

           anchors {
               top: title.bottom
               topMargin: units.gu(50)
           }
       }

       Item {
           id: divider

           anchors.top: loginPage.bottom
           height: units.gu(100)
           width: parent.width
       }

       Text {
           id: agree_text
           anchors.top: divider.bottom
           text: qsTr("By signing up, I agree to glimpse's")
           anchors.horizontalCenter: parent.horizontalCenter
           color: "#333333"
       }

       Text {
           id: agree_link
           anchors.top: agree_text.bottom
           anchors.topMargin: units.gu(10)
           text: qsTr("Terms of Service")
           anchors.horizontalCenter: parent.horizontalCenter
           color: "#3680ab"
       }
    }
}
