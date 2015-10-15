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

    function enableTrafficBudgetManager(state) {
        if (state == true) {
            grid2.enabled = true;
            grid2.opacity = 1;
            if (mobileMeasurementsCheckbox.checked) {
                grid4.enabled = true;
                grid4.opacity = 1;
            }
        } else {
            grid2.enabled = grid4.enabled = false;
            grid2.opacity = grid4.opacity = 0.5;
        }
    }

    function enableMobileMeasurements(state) {
        if (state == true) {
            if (trafficBudgetManagerCheckbox.checked) {
                grid4.enabled = true;
                grid4.opacity = 1;
            }
        } else {
            grid4.enabled = false;
            grid4.opacity = 0.5;
        }
    }

    Component.onCompleted: {
        enableMobileMeasurements(client.settings.mobileMeasurementsActive);
        enableTrafficBudgetManager(client.settings.trafficBudgetManagerActive);
    }

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
        spacing: 10
        anchors.top: parent.top
        anchors.topMargin: units.gu(50)
        anchors.left: parent.left
        anchors.right: parent.right

        Label {
            id: headline
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Configure your device")
            color: "#727277"
        }

        Label {
            id: subtitle
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            color: "#727277"
            wrapMode: Text.WordWrap
            text: qsTr("Let us know how much data volume per month you would like to donate to the GLIMPSE project.")
            font {
                pixelSize: units.gu(30)
            }
        }

        GridLayout {
            id: grid0
            anchors {
                left: parent.left
                right: parent.right
                leftMargin: units.gu(20)
                rightMargin: units.gu(20)
                //margins: units.gu(10)
            }
            columnSpacing: units.gu(50)
            columns: 2

            Label {
                id: deviceName
                text: qsTr("Device name")
                color: "#333333"
                font.pixelSize: units.gu(35)
            }

            TextField {
                id: deviceNameTextField
                text: client.settings.deviceName
                anchors{
                    left: deviceName.right
                    right: parent.right
                    leftMargin: units.gu(20)
                }
            }
        }

        GridLayout {
            id: grid1
            anchors {
                left: parent.left
                right: parent.right
                leftMargin: units.gu(20)
                rightMargin: units.gu(20)
                //margins: units.gu(10)
            }
            columnSpacing: units.gu(50)
            columns: 2

            Label {
                text: qsTr("Enable traffic budget manager")
                color: "#333333"
                font.pixelSize: units.gu(35)
            }

            CheckBox {
                id: trafficBudgetManagerCheckbox
                checked: client.settings.trafficBudgetManagerActive
                onCheckedChanged: enableTrafficBudgetManager(checked)
            }
        }

        Rectangle {
            height: 1
            color: "#e2e2e2"
            anchors {
                left: parent.left
                right: parent.right
                topMargin: units.gu(20)
                bottomMargin: units.gu(20)
                leftMargin: units.gu(40)
                rightMargin: units.gu(40)
            }
        }

        GridLayout {
            id: grid2
            anchors {
                left: parent.left
                right: parent.right
                leftMargin: units.gu(20)
                rightMargin: units.gu(20)
                //margins: units.gu(10)
            }
            columnSpacing: units.gu(50)
            columns: 1

            Label {
                text: qsTr("Max. monthly traffic volume: %1MB").arg(trafficSlider.value)
                wrapMode: Text.Wrap
                color: "#333333"
                font.pixelSize: units.gu(35)
                Layout.maximumWidth: Layout.minimumWidth
            }

            Slider {
                id: trafficSlider
                maximumValue: 1000.0
                minimumValue: 30.0
                value: client.settings.allowedTraffic / (1024 * 1024)
                stepSize: 10
                Layout.fillWidth: true
            }

        }

        Rectangle {
            height: 1
            color: "#e2e2e2"
            anchors {
                left: parent.left
                right: parent.right
                topMargin: units.gu(20)
                bottomMargin: units.gu(20)
                leftMargin: units.gu(40)
                rightMargin: units.gu(40)
            }
        }

        GridLayout {
            id: grid3
            anchors {
                left: parent.left
                right: parent.right
                leftMargin: units.gu(20)
                rightMargin: units.gu(20)
                //margins: units.gu(10)
            }
            columnSpacing: units.gu(50)
            columns: 2

            Label {
                text: qsTr("Allow mobile measurements")
                color: "#333333"
                font.pixelSize: units.gu(35)
            }

            CheckBox {
                id: mobileMeasurementsCheckbox
                checked: client.settings.mobileMeasurementsActive
                onCheckedChanged: enableMobileMeasurements(checked)
            }
        }

        GridLayout {
            id: grid4
            anchors {
                left: parent.left
                right: parent.right
                leftMargin: units.gu(20)
                rightMargin: units.gu(20)
                //margins: units.gu(10)
            }
            columnSpacing: units.gu(50)
            columns: 1

            Label {
                text: qsTr("Max. monthly mobile traffic volume: %1MB").arg(trafficMobileSlider.value)
                color: "#333333"
                font.pixelSize: units.gu(35)
            }

            Slider {
                id: trafficMobileSlider
                maximumValue: 500.0
                minimumValue: 5.0
                value: client.settings.allowedMobileTraffic / (1024 * 1024)
                stepSize: 1
                Layout.fillWidth: true
            }
        }

        Rectangle {
            height: 1
            color: "#e2e2e2"
            anchors {
                left: parent.left
                right: parent.right
                topMargin: units.gu(20)
                bottomMargin: units.gu(20)
                leftMargin: units.gu(40)
                rightMargin: units.gu(40)
            }
        }

        GridLayout {
            id: grid5
            anchors {
                left: parent.left
                right: parent.right
                leftMargin: units.gu(20)
                rightMargin: units.gu(20)
                //margins: units.gu(10)
            }
            columnSpacing: units.gu(50)
            columns: 1

            Label {
                text: qsTr("Keep logs for %1 days").arg(backlogSlider.value)
                color: "#333333"
                font.pixelSize: units.gu(35)
            }

            Slider {
                id: backlogSlider
                maximumValue: 30.0
                minimumValue: 0.0
                value: 5
                stepSize: 1
                Layout.fillWidth: true
            }
        }

        Rectangle {
            height: 1
            color: "#e2e2e2"
            anchors {
                left: parent.left
                right: parent.right
                topMargin: units.gu(20)
                bottomMargin: units.gu(20)
                leftMargin: units.gu(40)
                rightMargin: units.gu(40)
            }
        }

        GridLayout {
            id: grid6
            anchors {
                left: parent.left
                right: parent.right
                leftMargin: units.gu(20)
                rightMargin: units.gu(20)
                //margins: units.gu(10)
            }
            columnSpacing: units.gu(50)
            columns: 2

            Label {
                text: qsTr("Enable Google Analytics")
                color: "#333333"
                font.pixelSize: units.gu(35)
            }

            CheckBox {
                id: googleAnalyticsCheckbox
                checked: client.settings.googleAnalyticsActive
            }
        }

        Rectangle {
            height: 1
            color: "#e2e2e2"
            anchors {
                left: parent.left
                right: parent.right
                topMargin: units.gu(20)
                bottomMargin: units.gu(20)
                leftMargin: units.gu(40)
                rightMargin: units.gu(40)
            }
        }

        BigButton {
            text: qsTr("Register device")
            anchors.horizontalCenter: parent.horizontalCenter

            onClicked: {
                client.settings.allowedTraffic = trafficSlider.value * 1024 * 1024
                client.settings.allowedMobileTraffic = trafficMobileSlider.value * 1024 * 1024
                client.settings.backlog = backlogSlider.value
                client.settings.trafficBudgetManagerActive = trafficBudgetManagerCheckbox.checked
                client.settings.mobileMeasurementsActive = mobileMeasurementsCheckbox.checked
                client.settings.googleAnalyticsActive = googleAnalyticsCheckbox.checked
                client.settings.deviceName = deviceNameTextField.text
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
