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

        onPaint: {
            var chart = new LineChart.LineChart();
//            how many results should be shown in the graph
//            to show all uncommment the line below
            var numberOfResultsToShow = 3;
//            var numberOfResultsToShow = root.resultJSON.results.length;
            var pointJSON = new Array(numberOfResultsToShow);

            for (var i = 0; i < numberOfResultsToShow; i++) {
                pointJSON[i] = new Array(root.resultJSON.results[i].probe_result.length);
                for (var n = 0; n < root.resultJSON.results[i].probe_result.length; n++) {
                    pointJSON[i][n] = new Array(1);
                    pointJSON[i][n][0] = parseFloat(root.resultJSON.results[i].probe_result[n]);
                }
            }

            chart.width = width - 10;
            chart.matrix_textPaddingLeft = 0.7;
            chart.matrix_scale_round_digits = 1;
            chart.pointJSON = pointJSON;
            chart.graph_linePaddingLeft = 0.11;

            chart.drawLineChart(0, 0, getContext("2d"));
        }


    }
}
