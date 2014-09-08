import QtQuick 2.0
import QtQuick.Layouts 1.1
import mplane 1.0
import "controls"

Page {
    id: root

    property bool isWizard: false

    title: qsTr("Device registration")
    activity: requester.status == WebRequester.Running

    anchors.margins: units.gu(20)

    WebRequester {
        id: requester
        url: "https://%1".arg(client.settings.config.supervisorAddress)

        request: RegisterDeviceRequest {
            id: request
        }

        response: RegisterDeviceResponse {
            id: response
        }

        onStatusChanged: {
            switch (status) {
            case WebRequester.Running:
                console.log("Device Registration sent");
                break;
            }
        }

        onError: {
            console.log("Device Registration error: " + requester.errorString());
            errorLabel.text = requester.errorString();
        }
        onFinished: {
            console.log("Device Registration finished");
            client.taskController.fetchTasks();

            if ( root.isWizard )
                app.menuPage();
            else
                pageStack.pop();
        }
    }

    Column {
        id: column
        spacing: 20
        anchors.top: parent.top
        anchors.topMargin: units.gu(50)
        anchors.left: parent.left
        anchors.right: parent.right

        Label {
            id: headline
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Configure your device"
            color: "#727277"
        }

        Label {
            id: subtitle
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            color: "#727277"
            wrapMode: Text.WordWrap
            text: "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed dia."
            font {
                pixelSize: units.gu(30)
            }
        }

        Rectangle {
            id: hr
            height: units.gu(10)
            color: "#ff0000"
        }

        GridLayout {
            id: grid
            anchors {
                left: parent.left
                right: parent.right
                leftMargin: units.gu(20)
                rightMargin: units.gu(20)
                //margins: units.gu(10)
            }

            columns: 2

            Label {
                text: qsTr("monthly traffic \nusage: %1mb").arg(trafficSlider.value)
                wrapMode: Text.Wrap
                font {
                    pixelSize: units.gu(30)
                }
                color: "#727277"
            }

            Slider {
                id: trafficSlider
                maximumValue: 250.0
                minimumValue: 25.0
                value: 25.0
                stepSize: 1
                Layout.fillWidth: true
            }



            Label {
                text: qsTr("monthly traffic usage \n(mobile): %1mb").arg(trafficMobileSlider.value)
                font {
                    pixelSize: units.gu(30)
                }
                color: "#727277"
            }

            Slider {
                id: trafficMobileSlider
                maximumValue: 250.0
                minimumValue: 0.5
                value: 0.5
                stepSize: 0.5
                Layout.fillWidth: true
            }

            Label {
                text: "Country"
                font {
                    pixelSize: units.gu(30)
                }
                color: "#727277"
            }

            ComboBox {
                model: [ "Hirblingen", "Batzenhofen", "Teiting", "Aichach"]
                Layout.fillWidth: true
            }

            Label {
                text: "allow mobile \nmeasurements"
                font {
                    pixelSize: units.gu(30)
                }
                color: "#727277"
            }

            CheckBox {
                id: checkbox
                checked: true
            }
        }

        BigButton {
            text: qsTr("Register Device")
            anchors.horizontalCenter: parent.horizontalCenter

            onClicked: {
                requester.start()
            }
        }

        Text {
            id: errorLabel
            color: "red"
            font.pixelSize: units.gu(40)
            x: text.length > 0 ? parent.width/2-errorLabel.width/2 : parent.width
            width: root.width
            wrapMode: Text.Wrap

            Behavior on x {
                NumberAnimation {
                    easing.type: Easing.InBack
                    duration: 100
                }
            }
        }
    }
}
