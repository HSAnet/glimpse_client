import QtQuick 2.0
import QtQuick.Controls 1.0
import mplane 1.0
import "android"

Rectangle {
    width: 768
    height: 1280

    Flickable {
        anchors.fill: parent

        flickableDirection: Flickable.VerticalFlick
        contentWidth: parent.width
        //contentHeight:

        Image {
            id: logo
            anchors {
                top: parent.top
                topMargin: 100
                horizontalCenter: parent.horizontalCenter
            }

            source: "images/logo.png"
        }

        Component {
            id: loginFieldStyle

            TextFieldStyle {
                textColor: "#2989B2"
                font.pixelSize: 41

                padding {
                    left: 24
                    right: 24
                }
            }
        }

        Column {
            anchors {
                top: logo.bottom
                topMargin: 100
                horizontalCenter: parent.horizontalCenter
            }

            spacing: 48

            TextField {
                id: accountField
                placeholderText: qsTr("your@email.com")
                style: loginFieldStyle

                Component.onCompleted: {
                    __panel.placeholderTextColor = "#C9C9C9"
                }
            }

            TextField {
                id: passwordField
                placeholderText: qsTr("password")
                style: loginFieldStyle

                Component.onCompleted: {
                    __panel.placeholderTextColor = "#C9C9C9"
                }

                echoMode: TextInput.Password
            }

            Button {
                id: loginButton
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("login")
                style: ButtonStyle {}
            }
        }
    }
}
