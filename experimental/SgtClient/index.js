var Sgt = Sgt || {};

Sgt.SgtClient = (function() {
    var params = {
        iMaxPrecompute: 2000,
        tMaxPrecompute: 15000,
        useWebGl: true
    };

    var dom = {
        useSpring: $("#use_spring_layout"),
        showHeatmap: $("#show_heatmap"),
        selector: $("#select_matpower"),
        networkGraph: $("#sgt_network_graph"),
        networkHeatmap: $("#sgt_network_heatmap"),
        properties: $("#sgt_network_properties"),
        progressGroup: $("#sgt_progress_group"),
        progressMessage: $("#sgt_progress_message"),
        progress: $("#sgt_progress")
    };

    var graph = null;
    
    var layout = null;
    
    var graphics = null;

    var renderer = null;
    
    var editor = new JSONEditor(dom.properties[0], {mode : "tree"});

    var heatmap = h337.create({
        container: dom.networkHeatmap[0]
    });
    
    function loadNetwork(id) {
        removeGraph();
        loadNetwork.url = "http://sgt.com/api/networks/" + id;
        showProgress(true, "Loading network " + id + ". Please wait.");
        jQuery.getJSON(loadNetwork.url, networkLoaded);
    }

    function Vmag(bus) {
        var V = bus.bus.V;
        var VBase = bus.bus.V_base;
        var result = 0.0;
        for (var i = 0; i < V.length; ++i) {
            var VrPu = V[i].r / VBase;
            var ViPu = V[i].i / VBase;
            result += Math.sqrt(VrPu * VrPu + ViPu * ViPu);
        }
        return result;
    }

    function networkLoaded(netw) {

        var busses = netw.network.busses;
        var branches = netw.network.branches;

        graph = Viva.Graph.graph();

        busMap = {}; 

        for (var i = 0; i < busses.length; ++i)
        {
            var bus = busses[i];
            graph.addNode(bus.component.id, {VMag: Vmag(bus)});
            busMap[bus.component.id] = bus;
        }
        for (var i = 0; i < branches.length; ++i)
        {
            graph.addLink(branches[i].branch.bus0, branches[i].branch.bus1);
        }

        layout = Viva.Graph.Layout.forceDirected(graph, {
            springLength : 40,
            springCoeff : 0.0002,
            dragCoeff : 0.007,
            gravity : -0.5
            // theta : 1
        });

        if (params.useWebGl) {
            graphics = Viva.Graph.View.webglGraphics();
            var webglEvents = Viva.Graph.webglInputEvents(graphics, graph);
            oldEndRender = graphics.endRender;
            graphics.endRender = function() {
                oldEndRender();
                if (dom.showHeatmap[0].checked) drawHeatmap();
            }; 
        } else {
            graphics = Viva.Graph.View.svgGraphics();
            var webglEvents = null;
        }

        if (webglEvents) {
            webglEvents.mouseEnter(function (node) {
            }).mouseLeave(function (node) {
            }).dblClick(function (node) {
            }).click(function (node) {
                var url = loadNetwork.url + '/busses/' + node.id + '/properties/'
                $.ajax({
                    url: url,
                    async: false,
                    dataType: 'json',
                    success: function (response) {
                        editor.set(response);
                    }
                });
            });
        }

        renderer = Viva.Graph.View.renderer(graph, {
            layout   : layout,
            graphics   : graphics,
            renderLinks : true,
            prerender  : true,
            container  : document.getElementById("sgt_network_graph")
        });

        // we need to compute layout, but we don"t want to freeze the browser
        var tStart = new Date();
        var t = 0;
        showProgress(true, "Laying out network graph, please wait.");
        precompute(params.iMaxPrecompute, renderer.run);

        function precompute(iterations, callback) {
            var tCur = new Date();
            t = (tCur - tStart); // ms

            reportProgress(iterations, t);
            // let"s run 10 iterations per event loop cycle:
            var i = 0;
            while (iterations > 0 && i < 10) {
                layout.step();
                iterations--;
                i++;
            }
            if (iterations > 0 && t <= params.tMaxPrecompute) {
                setTimeout(function () {
                    precompute(iterations, callback);
                }, 0); // keep going in next even cycle
            } else {
                reportProgress(iterations, t);
                showProgress(false);
                syncHeatmap();
                callback();
                syncSpringLayout();
                drawHeatmap();
            }
        }
    }

    function removeGraph() {
        if (!renderer) {
            return; // already removed
        }
        renderer.dispose(); // remove the graph
        renderer = null;
    }

    var files = $.getJSON(
        "http://sgt.com/api/matpower_files/",
        function(files) {
            for (var i = 0; i < files.length; ++i) {
                dom.selector.append("<option>" + files[i] + "</option>");
            }
        }
    );

    dom.selector.change(
        function() {
            var file = $(this).find("option:selected").text();
            var url = "http://sgt.com/api/networks/" + file;
            var json = JSON.stringify({"matpower_filename": file});
            $.ajax({
                url: url,
                type: "PUT",
                contentType: "application/json",
                data: json,
                success: function(result) {
                    loadNetwork(file);
                }
            });
        }
    );

    function syncSpringLayout() {
        if (renderer) {
            if (dom.useSpring[0].checked) {
                renderer.resume();
            } else {
                renderer.pause();
            }
        }
    }

    function syncHeatmap() {
        if (dom.showHeatmap[0].checked) {
            drawHeatmap();
            dom.networkHeatmap[0].removeAttribute("hidden");
        } else {
            dom.networkHeatmap[0].setAttribute("hidden");
            clearHeatmap();
        }
    }

    function clearHeatmap() {
        heatmap.setData({min: 0.8, max: 1.2, data: []});
    }

    function drawHeatmap()
    {
        clearHeatmap();
        graph.forEachNode(function(node) {
            var pos = layout.getNodePosition(node.id);
            var newPos = graphics.transformGraphToClientCoordinates(
                {x: pos.x, y: pos.y, value: pos.value, reset: pos.reset, radius: 50}
            );
            newPos.value = node.data.VMag;
            heatmap.addData(newPos);
        });
    }

    function showProgress(isVisible, htmlMessage) {
        if (isVisible) {
            if (htmlMessage) {
                dom.progressMessage[0].innerHTML = htmlMessage;
            }
            dom.progress[0].value = -1;
            dom.progressGroup[0].removeAttribute("hidden");
        } else {
            dom.progressGroup[0].setAttribute("hidden");
        }
    }

    function reportProgress(iter, t) {
        var iterPercent = 100 * (params.iMaxPrecompute - iter) / params.iMaxPrecompute;
        var tPercent = 100 * t / params.tMaxPrecompute;
        var percent = Math.max(iterPercent, tPercent);
        dom.progress[0].value = percent;
    }

    return {"loadNetwork": loadNetwork, "syncSpringLayout": syncSpringLayout, "syncHeatmap": syncHeatmap};
}());
