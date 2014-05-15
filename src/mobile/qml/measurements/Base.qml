import QtQuick 2.0
import "../controls"
import "../"

Page {
//    width: 100
//    height: 62

    property string measurement
    property variant properties

    Component {
        id: labelComponent

        Label {
            property variant valueObject
            property variant valueFormat
        }
    }

    Component {
        id: columnComponent

        Column {}
    }

    Component {
        id: buttonComponent

        Button {}
    }

    function startMeasurement() {
        console.log("startMeasurement");

        var func = client[root.measurement];
        var params = [0];

        for(var i in grid.children) {
            var children = grid.children[i].children;

            if (!children.length)
                continue;

            var child = children.length === 1 ? children[0] : children[1];

            if (!child.value && !child.text)
                continue;

            params.push(child.text ? child.text : child.value);
        }

        console.log(params);

        func.apply(this, Array.prototype.slice.call(params, 1));
    }

    Component.onCompleted: {
        for(var i in root.properties) {
            var prop = root.properties[i];

            // Create the label
            labelComponent.createObject(grid, {"text": prop.label});

            var column = columnComponent.createObject(grid);
            var valueLabel;

            switch(prop.type) {
            case "Slider":
                valueLabel = labelComponent.createObject(column);
                break;
            }

            var component = Qt.createComponent(Qt.resolvedUrl("../controls/%1.qml".arg(prop.type)))
            var control = component.createObject(column, prop);

            if (valueLabel) {
                valueLabel.valueFormat = prop.format ? prop.format : "%1";
                valueLabel.valueObject = control;
                valueLabel.text = Qt.binding(function() {
                    return this.valueFormat.arg(this.valueObject.value)
                })
            }
        }

        var button = buttonComponent.createObject(grid, {"text":qsTr("Start %1").arg(root.measurement)});
        button.clicked.connect(root.startMeasurement);
    }

    Rectangle {
        id: listBackground
        width: app.width - units.gu(50)
        height: parent.height

        anchors {
            top: parent.top
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
            topMargin: units.gu(20)
            bottomMargin: units.gu(20)
        }

        Image {
            id: flap
            anchors.right: parent.right
            anchors.top: parent.top
            source: "../images/flap_small.png"
            height: units.gu(60)
            fillMode: Image.PreserveAspectFit
        }

        Grid {
            id: grid
            anchors.fill: listBackground
            anchors.margins: units.gu(10)

            columns: 2
            //spacing: units.gu(2)

            /*
            Label { text: qsTr("Host") }
            TextField {
                id: hostField
            }

            Label { text: qsTr("Count") }
            Column {
                Label {
                    text: countSlider.value
                }

                Slider {
                    id: countSlider
                    stepSize: 1.0
                    minimumValue: 1
                    maximumValue: 255
                    value: 4
                }
            }

            Label { text: qsTr("Timeout") }
            Column {
                Label {
                    text: qsTr("%1 milliseconds").arg(timeoutSlider.value)
                }

                Slider {
                    id: timeoutSlider
                    stepSize: 1.0
                    minimumValue: 50
                    maximumValue: 20000
                    value: 2000
                }
            }

            Label { text: qsTr("Interval") }
            Column {
                Label {
                    text: qsTr("%1 milliseconds").arg(intervalSlider.value)
                }

                Slider {
                    id: intervalSlider
                    stepSize: 1.0
                    minimumValue: 1
                    maximumValue: 20000
                    value: 200
                }
            }

            Item {}
            Button {
                text: qsTr("Start ping")
                onClicked: root.startMeasurement(hostField.text, countSlider.value, timeoutSlider.value, intervalSlider.value)
            }*/
        }
    }
}
