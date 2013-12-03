import QtQuick 2.0
import mplane 1.0
import "controls"

Page {
    id: root
    title: qsTr("Login")

    height: column.height

    property bool loginMode: true
    property string buttonTitle: qsTr("Login")

    activity: client.loginController.status == Controller.Running

    Connections {
        id: requester

        target: client.loginController

        onError: {
            console.log("Login error: " + target.errorString())
            errorLabel.text = target.errorString();
        }

        onFinished: {
            console.log("Login finished")

            if ( target.registeredDevice ) {
                console.log("Device is already registered");
                pageStack.pop();
            }
            else {
                var properties = {
                    item: Qt.resolvedUrl("DeviceRegistration.qml"),
                    replace: true
                }

                console.log("Pushing device registration")

                pageStack.push(properties);
            }
        }
    }

    Column {
        id: column
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: units.gu(10)

        Column {
            anchors.horizontalCenter: parent.horizontalCenter

            Text {
                text: qsTr("Mail")
                font.pixelSize: units.gu(40)
            }

            TextField {
                id: mailField
                text: client.settings.userId
                validator: RegExpValidator {
                    regExp: /.+@.+\..+/
                }
            }
        }

        Column {
            anchors.horizontalCenter: parent.horizontalCenter

            Text {
                text: qsTr("Password")
                font.pixelSize: units.gu(40)
            }

            TextField {
                id: passwordField
                text: client.settings.password
                echoMode: TextInput.Password
            }
        }

        Column {
            visible: !root.loginMode
            anchors.horizontalCenter: parent.horizontalCenter

            Text {
                text: qsTr("Confirm password")
                font.pixelSize: units.gu(40)
            }

            TextField {
                id: passwordField2
                echoMode: TextInput.Password
            }
        }

        Label {
            id: errorLabel
            color: "red"
            x: text.length > 0 ? parent.width/2-errorLabel.width/2 : parent.width

            Behavior on x {
                NumberAnimation {
                    easing.type: Easing.InBack
                    duration: 100
                }
            }
        }

        Button {
            text: root.buttonTitle
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: {
                if ( !mailField.acceptableInput ) {
                    errorLabel.text = qsTr("Mail is not valid");
                    return;
                }

                if ( passwordField.text.length < 6 ) {
                    errorLabel.text = qsTr("Password must be 6 characters or more");
                    return;
                }

                if ( !root.loginMode && passwordField.text != passwordField2.text ) {
                    errorLabel.text = qsTr("Passwords don't match");
                    return;
                }

                if (root.loginMode) {
                    client.settings.userId = mailField.text;
                    client.settings.password = passwordField.text;
                    client.loginController.login();
                } else {
                    client.loginController.registration(mailField.text, passwordField.text);
                }
            }
        }
    }
}
