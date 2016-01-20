var renderer = null;
var iMaxPrecompute = 2000;
var tMaxPrecompute = 15000; // ms
var useWebGl = true;
    
function loadNetwork(id) {
    removeGraph();
    var url = 'http://sgt.com/api/networks/' + id;
    showProgress(true, 'Loading network ' + id + '. Please wait.');
    jQuery.getJSON(url, graphLoaded);
}

function graphLoaded(netw) {

    var busses = netw.network.busses;
    var branches = netw.network.branches;

    var graph = Viva.Graph.graph();

    busMap = {}; 

    for (i = 0; i < busses.length; ++i)
    {
        var bus = busses[i];
        graph.addNode(bus.component.id);
        busMap[bus.component.id] = bus;
    }
    for (i = 0; i < branches.length; ++i)
    {
        graph.addLink(branches[i].branch.bus0, branches[i].branch.bus1);
    }

    var layout = Viva.Graph.Layout.forceDirected(graph, {
        springLength : 40,
        springCoeff : 0.0002,
        dragCoeff : 0.007,
        gravity : -0.5
        // theta : 1
    });

    if (useWebGl) {
        var graphics = Viva.Graph.View.webglGraphics();
        var webglEvents = Viva.Graph.webglInputEvents(graphics, graph);
    } else {
        var graphics = Viva.Graph.View.svgGraphics();
        var webglEvents = null;
    }

    if (webglEvents) {
        webglEvents.mouseEnter(function (node) {
            console.log('Mouse entered node: ' + node.id);
        }).mouseLeave(function (node) {
            console.log('Mouse left node: ' + node.id);
        }).dblClick(function (node) {
            console.log('Double click on node: ' + node.id);
        }).click(function (node) {
            console.log('Single click on node: ' + node.id);
            var busJson = busMap[node.id];
            $('#sgt_network_properties').jsonEditor(busJson);
        });
    }

    renderer = Viva.Graph.View.renderer(graph, {
        layout   : layout,
        graphics   : graphics,
        renderLinks : true,
        prerender  : true,
        container  : document.getElementById('sgt_network_graph')
    });

    // we need to compute layout, but we don't want to freeze the browser
    var tStart = new Date();
    var t = 0;
    showProgress(true, 'Laying out network graph, please wait.');
    precompute(iMaxPrecompute, renderer.run);

    function precompute(iterations, callback) {
        var tCur = new Date();
        t = (tCur - tStart); // ms

        reportProgress(iterations, t);
        // let's run 10 iterations per event loop cycle:
        var i = 0;
        while (iterations > 0 && i < 10) {
            layout.step();
            iterations--;
            i++;
        }
        if (iterations > 0 && t <= tMaxPrecompute) {
            setTimeout(function () {
                precompute(iterations, callback);
            }, 0); // keep going in next even cycle
        } else {
            reportProgress(iterations, t);
            showProgress(false);
            callback();
            syncSpringLayout();
        }
    }
};

function removeGraph() {
    if (!renderer) {
        return; // already removed
    }
    renderer.dispose(); // remove the graph
    renderer = null;
}

var selector = $('#select_matpower');
var files = $.getJSON(
    'http://sgt.com/api/matpower_files/',
    function(files) {
        for (i = 0; i < files.length; ++i) {
            selector.append('<option>' + files[i] + '</option>');
        }
    }
);

selector.change(
    function() {
        var file = $(this).find('option:selected').text();
        var url = 'http://sgt.com/api/networks/' + file;
        var json = JSON.stringify({'matpower_filename': file});
        $.ajax({
            url: url,
            type: 'PUT',
            contentType: 'application/json',
            data: json,
            success: function(result) {
                loadNetwork(file);
            }
        });
    }
);

function syncSpringLayout() {
    var useSpringLayout = springLayoutIsOn();
    if (renderer) {
        if (useSpringLayout) {
            renderer.resume();
        } else {
            renderer.pause();
        }
    }
}

function springLayoutIsOn() {
    var chk = $('#use_spring_layout')[0];
    return chk.checked;
}

function showProgress(isVisible, htmlMessage) {
    var progressGroupSel = $('#sgt_progress_group');
    var progressGroup = progressGroupSel[0];
    var progressMessage = progressGroupSel.find('#sgt_progress_message')[0];
    var progress = progressGroupSel.find('#sgt_progress')[0];
    if (isVisible) {
        if (htmlMessage) {
            progressMessage.innerHTML = htmlMessage;
        }
        progress.value = -1;
        progressGroup.removeAttribute('hidden');
    } else {
        progressGroup.setAttribute('hidden');
    }
}

function reportProgress(iter, t) {
    var iterPercent = 100 * (iMaxPrecompute - iter) / iMaxPrecompute;
    var tPercent = 100 * t / tMaxPrecompute;
    var percent = Math.max(iterPercent, tPercent);
    reportProgress.progress.value = percent;
}

$(document).ready(function() {
    reportProgress.progress = $('#sgt_progress')[0];
});
