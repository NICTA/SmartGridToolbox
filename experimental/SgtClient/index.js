var Sgt = Sgt || {};

Sgt.SgtClient = (function() {
    var params = {
        iMaxPrecompute: 1000,
        tMaxPrecompute: 10000,
        useWebGl: true,
        VLow: 0.95,
        VHigh: 1
    };
    params.VRange = params.VHigh - params.VLow;

    var dom = {
        useSpring: $("#use-spring-layout"),
        showHeatmap: $("#show-heatmap"),
        selector: $("#select-matpower"),
        networkGraph: $("#sgt-network-graph"),
        networkHeatmap: $("#sgt-heatmap-canvas"),
        properties: $("#sgt-network-properties"),
        progressGroup: $("#sgt-progress-group"),
        progressMessage: $("#sgt-progress-message"),
        progress: $("#sgt-progress")
    };

    var graph = null;
    
    var layout = null;
    
    var graphics = null;

    var renderer = null;
    
    var editor = new JSONEditor(dom.properties[0], {mode : "tree"});

    Dexter.Heatmap.init(dom.networkHeatmap[0]);
    Dexter.Heatmap.setViewRectToCanvas();
        
    function clamp(x, min, max) {
        return Math.max(min, Math.min(x, max));
    }

    function VMag(bus) {
        var V = bus.bus.V;
        var VBase = bus.bus.V_base;
        var result = 0.0;
        for (var i = 0; i < V.length; ++i) {
            var Vr = V[i].r;
            var Vi = V[i].i;
            result += Math.sqrt(Vr * Vr + Vi * Vi);
        }
        result /= VBase;
        return result;
    }

    function loadNetwork(id) {
        removeGraph();
        loadNetwork.url = "http://sgt.com/api/networks/" + id;
        showProgress(true, "Loading network " + id + ". Please wait.");
        jQuery.getJSON(loadNetwork.url, networkLoaded);
    }

    function networkLoaded(netw) {

        var busses = netw.network.busses;
        var branches = netw.network.branches;

        graph = Viva.Graph.graph();

        busMap = {}; 

        for (var i = 0; i < busses.length; ++i)
        {
            var bus = busses[i];
            graph.addNode(bus.component.id, {VMag: VMag(bus)});
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
            container  : document.getElementById("sgt-network-graph")
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
        if (dom.showHeatmap[0].checked && graph) {
            dom.networkHeatmap[0].removeAttribute("hidden");
        } else {
            dom.networkHeatmap[0].setAttribute("hidden");
        }
    }

    function drawHeatmap()
    {
        dat = [];
        graph.forEachNode(function(node) {
            var pos = layout.getNodePosition(node.id);
            var newPos = graphics.transformGraphToClientCoordinates({x: pos.x, y: pos.y});
            var VParam = (clamp(node.data.VMag, params.VLow, params.VHigh) - params.VLow) / params.VRange;
            dat.push([[newPos.x, newPos.y], VParam]);
        });
        Dexter.Heatmap.setData(dat);
        Dexter.Heatmap.draw(); // adds the buffered points
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
