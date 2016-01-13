var graph = Viva.Graph.graph();

var layout = Viva.Graph.Layout.forceDirected(graph, {
    springLength : 30,
    // springCoeff : 0.0001,
    dragCoeff : 0.05,
    gravity : -1.2
    // theta : 1
});

var graphics = Viva.Graph.View.webglGraphics();

var renderer = Viva.Graph.View.renderer(
    graph, 
    {
        layout   : layout,
        graphics   : graphics,
        renderLinks : true,
        prerender  : true,
        container  : document.getElementById('network_graph')
    }
);

function loadNetwork(id) {
    var url = "http://localhost:34568/networks/" + id;
    var width = 960;
    var height = 500;

    console.log("Load network " + url);

    jQuery.getJSON(url, drawGraph);

    function drawGraph(netw) {
        var busses = netw.network.busses;
        var branches = netw.network.branches;

        graph.clear();
        for (i = 0; i < busses.length; ++i)
        {
            graph.addNode(busses[i].component.id);
        }
        for (i = 0; i < branches.length; ++i)
        {
            graph.addLink(branches[i].branch.bus0, branches[i].branch.bus1);
        }

        // we need to compute layout, but we don't want to freeze the browser
        precompute(1000, renderGraph);

        function precompute(iterations, callback) {
            // let's run 10 iterations per event loop cycle:
            var i = 0;
            while (iterations > 0 && i < 10) {
                layout.step();
                iterations--;
                i++;
            }
            console.log('Layout precompute: ' + iterations);
            if (iterations > 0) {
                setTimeout(function () {
                    precompute(iterations, callback);
                }, 0); // keep going in next even cycle
            } else {
                // we are done!
                callback();
            }
        }

        function renderGraph() {
            console.log('renderGraph');

            renderer.settings = {
                layout   : layout,
                graphics   : graphics,
                renderLinks : true,
                prerender  : true,
                container  : document.getElementById('network_graph')
            };

            renderer.run();

            // Final bit: most likely graph will take more space than available
            // screen. Let's zoom out to fit it into the view:
            var graphRect = layout.getGraphRect();
            var graphSize = Math.max(graphRect.x2 - graphRect.x1, graphRect.y2 - graphRect.y1);
            var screenSize = Math.min($('#network_graph').width(), $('#network_graph').height());

            var desiredScale = screenSize / graphSize;
            zoomOut(desiredScale, 1);

            function zoomOut(desiredScale, currentScale) {
                // zoom API in vivagraph 0.5.x is silly. There is no way to pass transform
                // directly. Maybe it will be fixed in future, for now this is the best I could do:
                if (desiredScale < currentScale) {
                    currentScale = renderer.zoomOut();
                    setTimeout(function () {
                        zoomOut(desiredScale, currentScale);
                    }, 16);
                }
            }
        }
    }
};

var selector = $("#select_matpower");
var files = $.getJSON('http://localhost:34568/matpower_files/', function(files) {
    for (i = 0; i < files.length; ++i)
    {
        selector.append("<option>" + files[i] + '</option>');
    }
});

selector.change(
    function() {
        var file = $(this).find("option:selected").text();
        var url = 'http://localhost:34568/networks/' + file;
        var json = JSON.stringify({"matpower_filename": file});
        $.ajax({
            url: url,
            type: 'PUT',
            contentType: "application/json",
            data: json,
            success: function(result) {
                loadNetwork(file);
            }
        });
    }
);
