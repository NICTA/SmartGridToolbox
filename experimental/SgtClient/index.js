var Sgt = Sgt || {};

Sgt.SgtClient = (function() {
    var params = {
        iMaxPrecompute: 1000,
        tMaxPrecompute: 10000,
        useWebGl: true,
    };

    var dom = {
        selector: $("#select-matpower"),
        useSpring: $("#use-spring-layout"),
        showHeatmap: $("#show-heatmap"),
        rangeVLow: $("#range-V-low"),
        rangeVHigh: $("#range-V-high"),
        labelVLow: $("#label-V-low"),
        labelVHigh: $("#label-V-high"),
        networkGraph: $("#sgt-network-graph"),
        heatmapCanvas: $("#sgt-heatmap-canvas"),
        labelCanvas: $("#sgt-label-canvas"),
        properties: $("#sgt-network-properties"),
        progressGroup: $("#sgt-progress-group"),
        progressMessage: $("#sgt-progress-message"),
        progress: $("#sgt-progress")
    };

    var graph = null;
    
    var layout = null;
    
    var graphics = null;

    var renderer = null;
   
    dom.labelCanvas[0].setAttribute('width', dom.labelCanvas[0].offsetWidth);
    dom.labelCanvas[0].setAttribute('height',dom.labelCanvas[0].offsetHeight);
    var labelCtx = dom.labelCanvas[0].getContext("2d");
    labelCtx.textAlign = "center";

    var VLow = 0.0;
    var VHigh = 2.0;
    var VRange;
    syncVLow();
    syncVHigh();
   
    Dexter.Heatmap.init(dom.heatmapCanvas[0]);
    Dexter.Heatmap.setViewRectToCanvas();
        
    function clamp(x, min, max) {
        return Math.max(min, Math.min(x, max));
    }

    function VMag(bus) {
        var V = bus.bus.V;
        var VBase = bus.bus.V_base;
        var result = 0.0;
        for (var i = 0; i < V.length; ++i) {
            var Vr = V[i][0];
            var Vi = V[i][1];
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
            graph.addNode(bus.component.id, {VMag: VMag(bus), type: bus.bus.type});
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
        } else {
            graphics = Viva.Graph.View.svgGraphics();
            var webglEvents = null;
        }

        graphics.node(function (node) {
            switch(node.data.type) {
                case "SL": 
                    col = 0xaa0000ff; 
                    break;
                case "PV": 
                    col = 0xaaaa00ff; 
                    break;
                case "PQ":
                    col = 0x0000aaff; 
                    break;
            }
            return {size: 10, color: col};
        });

        oldEndRender = graphics.endRender;
        graphics.endRender = function() {
            oldEndRender();
            if (dom.showHeatmap[0].checked) drawHeatmap();
            drawLabels();
        }; 

        if (webglEvents) {
            webglEvents.mouseEnter(function (node) {
            }).mouseLeave(function (node) {
            }).dblClick(function (node) {
            }).click(function (node) {
                var url = loadNetwork.url + "/busses/" + node.id + "/properties/"
                $.ajax({
                    url: url,
                    async: false,
                    dataType: "json",
                    success: function (response) {
                        dom.properties.jqPropertyGrid(response, {
                            "id": {group: "Identity"},
                            "type": {group: "Identity"},
                            "phases": {group: "Identity"},
                            "V": {group: "Key Bus Properties"},
                            "VBase": {group: "Key Bus Properties"},
                            "isInService": {group: "Key Bus Properties"},
                            "nInServiceGens": {group: "Key Bus Properties"},
                            "SGen": {group: "Key Bus Properties"},
                            "nInServiceZips": {group: "Key Bus Properties"},
                            "SZip": {group: "Key Bus Properties"}
                        });
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

    function drawHeatmap()
    {
        dat = [];
        graph.forEachNode(function(node) {
            var pos = layout.getNodePosition(node.id);
            if (params.useWebGl) {
                var newPos = graphics.transformGraphToClientCoordinates({x: pos.x, y: pos.y});
            } else {
                var svg = $("g")[0];
                var t = svg.getCTM();
                var newPos = {x: t.a * pos.x + t.b * pos.y + t.e, y: t.c * pos.x + t.d * pos.y + t.f};
            }
            var VParam = (clamp(node.data.VMag, VLow, VHigh) - VLow) / VRange;
            dat.push([[newPos.x, newPos.y], VParam]);
        });
        Dexter.Heatmap.setData(dat);
        Dexter.Heatmap.draw(); // adds the buffered points
    }

    function drawLabels()
    {
        labelCtx.clearRect(0, 0, dom.labelCanvas[0].scrollWidth, dom.labelCanvas[0].scrollHeight);
        graph.forEachNode(function(node) {
            var pos = layout.getNodePosition(node.id);
            var newPos = graphics.transformGraphToClientCoordinates({x: pos.x, y: pos.y + 10});
            labelCtx.fillText(node.id, newPos.x, newPos.y);
        });
    }

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
            dom.heatmapCanvas[0].removeAttribute("hidden");
        } else {
            dom.heatmapCanvas[0].setAttribute("hidden");
        }
    }

    function syncVLow() {
        VLow = dom.rangeVLow[0].value / 100;
        VHigh = dom.rangeVHigh[0].value / 100;
        if (VLow > VHigh) {
            VLow = VHigh;
            dom.rangeVLow[0].value = 100 * VLow;
        }
        dom.labelVLow[0].innerHTML = VLow;
        VRange = VHigh - VLow;
        if (dom.showHeatmap[0].checked) drawHeatmap();
    }

    function syncVHigh() {
        VLow = dom.rangeVLow[0].value / 100;
        VHigh = dom.rangeVHigh[0].value / 100;
        if (VHigh < VLow) {
            VHigh = VLow;
            dom.rangeVHigh[0].value = 100 * VHigh;
        }
        dom.labelVHigh[0].innerHTML = VHigh;
        VRange = VHigh - VLow;
        if (dom.showHeatmap[0].checked) drawHeatmap();
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

    return {
        "loadNetwork": loadNetwork,
        "syncSpringLayout": syncSpringLayout,
        "syncHeatmap": syncHeatmap,
        "syncVLow": syncVLow,
        "syncVHigh": syncVHigh
    };
}());
