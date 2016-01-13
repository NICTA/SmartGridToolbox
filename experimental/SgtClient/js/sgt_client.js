var renderer = null;

function removeGraph() {
    if (!renderer) {
        return; // already removed
    }
    renderer.dispose(); // remove the graph
    renderer = null;
}

function loadNetwork(id) {
    console.log("Load network " + url);
    
    var url = "http://localhost:34568/networks/" + id;
    var width = 960;
    var height = 500;

    jQuery.getJSON(url, drawGraph);
}

function drawGraph(netw) {
    removeGraph();

    var busses = netw.network.busses;
    var branches = netw.network.branches;

    var graph = Viva.Graph.graph();

    for (i = 0; i < busses.length; ++i)
    {
        graph.addNode(busses[i].component.id);
    }
    for (i = 0; i < branches.length; ++i)
    {
        graph.addLink(branches[i].branch.bus0, branches[i].branch.bus1);
    }

    var layout = Viva.Graph.Layout.forceDirected(graph, {
        springLength : 30,
        // springCoeff : 0.0001,
        dragCoeff : 0.05,
        gravity : -1.2
        // theta : 1
    });

    var graphics = Viva.Graph.View.webglGraphics();

    renderer = Viva.Graph.View.renderer(graph, {
        layout   : layout,
        graphics   : graphics,
        renderLinks : true,
        prerender  : true,
        container  : document.getElementById('network_graph')
    });
    
    // we need to compute layout, but we don't want to freeze the browser
    precompute(500, renderer.run);

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
            callback();
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
