import QtQuick 2.0
import "controls"
import "controls/charts/linechart.js" as LineChart

Page {
    id: root
    title: qsTr("report diagram")
    subtitle: qsTr("report visualistion")

    property variant resultJSON

    Canvas {
        anchors.fill: parent
        width: parent.width - units.gu(50)
        height: 220

        Component.onCompleted: {
            console.log(root.resultJSON)
            //console.log(root.resultJSON.results)
        }

        onPaint: {
            var chart = new LineChart.Chart();
            chart.width = 400;
            chart.pointJSON = root.resultJSON.results[0].probe_result[0];
            chart.drawLineChart(0, 0, getContext("2d"));

        }


    }
}
