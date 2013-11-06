import QtQuick 2.0
import mplane 1.0
import "android"
import "controls"

Page {
    id: root
    title: qsTr("Login")

    property variant request: LoginRequest {
    }

    property string buttonTitle: qsTr("Login")

    activity: requester.status == WebRequester.Running

    anchors.margins: units.gu(20)

    WebRequester {
        id: requester

        request: root.request

        response: LoginResponse {
            id: response
        }

        onStarted: {
            request.userId = mailField.text;
            request.password = passwordField.text;

            // Store these values for later use
            client.settings.userId = request.userId
            client.settings.password = request.password

            console.log("Login sent")
        }

        onError: {
            console.log("Login error: " + requester.errorString())
            errorLabel.text = requester.errorString();
        }

        onFinished: {
            console.log("Login finished")

            if ( response.registeredDevice ) {
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
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: units.gu(10)

        Column {
            anchors.horizontalCenter: parent.horizontalCenter

            Text {
                text: "Mail"
                font.pixelSize: units.gu(40)
            }

            TextField {
                id: mailField
                style: TextFieldStyle {}
                text: client.settings.userId
                validator: RegExpValidator {
                    regExp: /.+@.+\..+/
                }
            }
        }

        Column {
            anchors.horizontalCenter: parent.horizontalCenter

            Text {
                text: "Password"
                font.pixelSize: units.gu(40)
            }

            TextField {
                id: passwordField
                style: TextFieldStyle {}
                text: client.settings.password
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

                requester.start()
            }
        }
    }
}
