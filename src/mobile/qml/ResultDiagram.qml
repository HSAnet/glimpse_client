import QtQuick 2.0
import "controls"
import "controls/charts/linechart.js" as LineChart


Page {
    id: root
    title: qsTr("report diagram")
    subtitle: qsTr("report visualistion")

    property variant resultJSON

    Canvas {
        id: canvas
        width: parent.width - units.gu(50)
        height: 220

        onPaint: {
            var line_chart = new LineChart.LineChart();
            var pointJSON = new Array(1);
            pointJSON[0] = new Array(1);

            for (var n = 0; n < root.resultJSON.results.length; n++) {
                pointJSON[0][n] = new Array(1);
                pointJSON[0][n][0] = root.resultJSON.results[n][9];
            }
            line_chart.pointJSON = pointJSON;
            line_chart.drawLineChart(0, 0, getContext("2d"));
        }
    }
}
