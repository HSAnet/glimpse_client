import QtQuick 2.0
import QtQuick.Layouts 1.1
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

        BigButton {}
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

            if (!child.value && !child.text && !child.hasOwnProperty("checked"))
                continue;

            params.push(child.hasOwnProperty("checked") ? child.checked : (child.text ? child.text : child.value));
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
            column.Layout.fillWidth = true;
            var valueLabel;

            switch(prop.type) {
            case "Slider":
                valueLabel = labelComponent.createObject(column);
                break;
            }

            var component = Qt.createComponent(Qt.resolvedUrl("../controls/%1.qml".arg(prop.type)))
            var control = component.createObject(column, prop);
            control.width = Qt.binding(function() {
                return this.parent.width;
            });

            if (valueLabel) {
                valueLabel.valueFormat = prop.format ? prop.format : "%1";
                valueLabel.valueObject = control;
                valueLabel.text = Qt.binding(function() {
                    return this.valueFormat.arg(this.valueObject.value)
                })
            }
        }

        var button = buttonComponent.createObject(grid, {"text":qsTr("Start")});
        button.Layout.columnSpan = 2;
        button.Layout.alignment = Qt.AlignHCenter;
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

        GridLayout {
            id: grid
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                topMargin: units.gu(40)
                leftMargin: units.gu(20)
                rightMargin: units.gu(20)
                //margins: units.gu(10)
            }

            columns: 2
        }
    }
}
