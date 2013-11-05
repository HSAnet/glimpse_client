import QtQuick 2.0
import QtQuick.Controls 1.0
import mplane 1.0
import "android"

ListView {
    id: root
    property string title: "Device registration"
    property string subtitle: "blah ..."

    anchors.margins: units.gu(20)
    spacing: units.gu(10)

    WebRequester {
        id: requester

        request: RegisterDeviceRequest {
            id: request
        }

        response: RegisterDeviceResponse {
            id: response
        }

        onStatusChanged: {
            switch(status) {
            case WebRequester.Running:
                console.log("Device Registration sent")
                break;
            }
        }

        onError: {
            console.log("Device Registration error: " + requester.errorString())
            errorLabel.text = requester.errorString();
        }
        onFinished: {
            console.log("Device Registration finished")
            pageStack.pop();
        }
    }

    model: VisualItemModel {
        Button {
            text: "Register Device"
            anchors.horizontalCenter: parent.horizontalCenter
            style: ButtonStyle {}
            onClicked: {
                requester.start()
            }
        }

        Spinner {
            anchors.horizontalCenter: parent.horizontalCenter
            running: requester.running
        }
    }
}
