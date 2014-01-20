import QtQuick 2.0
import mplane 1.0
import QtQuick.Controls 1.0
import QtQuick.Window 2.0
import "android"

Item {
    width: 400
    height: 500

    property string title: qsTr("Manual tests")
    property string subtitle: qsTr("Listing all tests")

    WebRequester {
        id: manualRequest
        request: TestRequest {
        }

        onStatusChanged: {
            switch (status) {
            case WebRequester.Running:
                statusText.text = qsTr("Requesting test ...");
                break;

            case WebRequester.Error:
                statusText.text = qsTr("Error: %1").arg(errorString());
                break;
            }
        }
    }

    ListView {
        anchors.fill: parent

        model: ListModel {
            ListElement {
                title: "Speedtest"
                testName: "speedtest"
                local: true
            }

            ListElement {
                title: "Packet train"
                testName: "packettrain"
                local: false
            }
        }

        delegate: AndroidDelegate {
            text: title
            onClicked: {
                if ( model.local )
                    client.speedTest();
                else
                    manualRequest.start()
            }
        }
    }
}
