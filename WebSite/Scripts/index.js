//var randomScalingFactor = function () { return Math.round(Math.random() * 100) };
//var lineChartData = {
//    labels: ["January", "February", "March", "April", "May", "June", "July"],
//    datasets: [
//    {
//        label: "My First dataset",
//        fillColor: "rgba(220,220,220,0.2)",
//        strokeColor: "rgba(220,220,220,1)",
//        pointColor: "rgba(220,220,220,1)",
//        pointStrokeColor: "#fff",
//        pointHighlightFill: "#fff",
//        pointHighlightStroke: "rgba(220,220,220,1)",
//        data: [randomScalingFactor(), randomScalingFactor(), randomScalingFactor(), randomScalingFactor(), randomScalingFactor(), randomScalingFactor(), randomScalingFactor()]
//    },
//    {
//        label: "My Second dataset",
//        fillColor: "rgba(151,187,205,0.2)",
//        strokeColor: "rgba(151,187,205,1)",
//        pointColor: "rgba(151,187,205,1)",
//        pointStrokeColor: "#fff",
//        pointHighlightFill: "#fff",
//        pointHighlightStroke: "rgba(151,187,205,1)",
//        data: [randomScalingFactor(), randomScalingFactor(), randomScalingFactor(), randomScalingFactor(), randomScalingFactor(), randomScalingFactor(), randomScalingFactor()]
//    }
//    ]
//}

$(document).ready(function () {
    $('html').removeClass();
    //$("#graph").hide();

    // Compute Price At 0
    $('#computePriceAt0').click(function () {
        $('#resPriceAt0').html("");

        // Get and Display
        $.getJSON("/Home/ComputePriceAt0", null, function (data) {
            $("#resPriceAt0").html(displayTabPrice(data));
        });
    });
    
    // Compute Price Now
    $('#computePriceNow').click(function () {
        $('#resPriceNow').html("");

        // Get and Display
        $.getJSON("/Home/ComputePriceNow", null, function (data) {
            $("#resPriceNow").html(displayTabPrice(data));
        });
    });

    // Get Last Hedge
    // dislay delta
    $('#getLastHedge').click(function () {
        $('#resHedge').html("");

        // Display the tab
        $.getJSON("/Home/GetLastHedge", null, function (data) {
            $("#resHedge").html(displayTableDateAssetValue(data));
        });
    });

    // Compute Hedge
    // dislay delta
    $('#computeHedge').click(function () {
        $('#resHedge').html("");

        // Display the tab
        $.getJSON("/Home/ComputeHedge", null, function (data) {
            $("#resHedge").html(displayTableDateAssetValue(data));
        });
    });

    // Graph : historical price
    /*
    $('#getGraph').click(function () {
        $('#resGraph').html("");

        // Display the graph
        $.getJSON("/Home/GetHistoricalPrice", null, function (data) {
            var html = "";
            var j = 1;
            var labels = [];
            var data1 = [];
            var data2 = [];
            $.each(data, function (i, item) {
                console.log(item);
                html += "<div>" + item.date + " : " + item.price + "</div>";
                labels.push(item.date);
                data1.push(item.price);
                data2.push(item.price + 1);
            });
            $("#resGraph").html(html);

            lineChartData = {
                labels: labels,
                datasets: [
                {
                    label: "My First dataset",
                    fillColor: "rgba(220,220,220,0.2)",
                    strokeColor: "rgba(220,220,220,1)",
                    pointColor: "rgba(220,220,220,1)",
                    pointStrokeColor: "#fff",
                    pointHighlightFill: "#fff",
                    pointHighlightStroke: "rgba(220,220,220,1)",
                    data: data1
                },
                {
                    label: "My Second dataset",
                    fillColor: "rgba(151,187,205,0.2)",
                    strokeColor: "rgba(151,187,205,1)",
                    pointColor: "rgba(151,187,205,1)",
                    pointStrokeColor: "#fff",
                    pointHighlightFill: "#fff",
                    pointHighlightStroke: "rgba(151,187,205,1)",
                    data: data2
                }
                ]
            };

            $("#graph").show();
            var ctx = document.getElementById("graph").getContext("2d");
            window.myLine = new Chart(ctx).Line(lineChartData, {
                responsive: true
            });
        });
    });
    */
});

// For price
function displayTabPrice(data) {
    var html = "";
    var j = 1;
    $.each(data, function (i, item) {
        html += "<tr class=\"tr" + j + "\">";
        html += "<td class=\"col1\">" + i + "</td>";
        html += "<td>" + item + "</td>";
        html += "</tr>";

        j = (j % 2) + 1;
    });

    return html;
}

// For hedge
function displayTableDateAssetValue(data) {
    var html = "";
    var nbDate = 0;
    var assets = [];

    $.each(data, function (i, item) {

        if (i == "date") {
            html += "<tr class=\"tr0\">";
            html += "<th>Assets \\ Date</th>";
            $.each(item, function (i2, item2) {
                html += "<th>" + item2.date + "</th>";
                nbDate++;
            });
            html += "</tr>";

        } else if (i == "asset") {
            $.each(item, function (i2, item2) {
                assets.push(item2.name);
            });

        } else if (i == "value") {
            var j = 0;
            var color = 2;
            $.each(item, function (i2, item2) {
                if (j % nbDate == 0) {
                    color = (color % 2) + 1;
                    html += "<tr class=\"tr" + color + "\"> <td class=\"col1\">" + assets.shift() + "</td>";
                }

                html += "<td>" + item2.value + "</td>";

                j++;
                if (j % nbDate == 0)
                    html += "</tr>";
            });
        }
    });

    return html;
}