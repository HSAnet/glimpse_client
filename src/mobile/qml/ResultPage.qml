import QtQuick 2.0

FlickablePage {
    id: root
    title: qsTr("report")
    subtitle: qsTr("see what happened recently")
    actionTitle: "Diagram"

    property alias resultText: textView.text
    property variant resultJSON

    function actionClicked(){

        var params = {
            item: Qt.resolvedUrl("ResultDiagram.qml"),
            properties: {
                resultJSON: root.resultJSON
            }
        }

        pageStack.push(params);
    }

    contentHeight: listBackground.height

    Rectangle {
        id: listBackground
        width: app.width - units.gu(50)
        height: column.height + units.gu(100)

        anchors {
            top: parent.top
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
            topMargin: units.gu(20)
            bottomMargin: units.gu(20)
        }

        Column {
            id: column
            width: parent.width

            Image {
                id: flap
                anchors.right: parent.right
                //anchors.top: parent.top
                source: "images/flap_small.png"
                height: units.gu(60)
                fillMode: Image.PreserveAspectFit
            }


            Text {
                id: textView
                color: "#333333"
                width: listBackground.width
                font.family: "Roboto Light"
                font.pixelSize: units.gu(20)
                renderType: Text.NativeRendering
            }
        }
    }
}
