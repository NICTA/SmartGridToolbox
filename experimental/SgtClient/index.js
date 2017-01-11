var Sgt = Sgt || {};

Sgt.SgtClient = (function() {
    var params = {
        iMaxPrecompute: 2000,
        tMaxPrecompute: 10000,
        nominalEdgeLength: 50,
        busSz: 10,
        genSz: 15,
        zipSz: 5,
        usePinNodes: false,
        url: "http://sgt.com/api",
        keysBranch: ["id", "componentType", "phases0", "phases1", "Y", "userData"],
        keysBus: [
            "id", "componentType", "phases", "V", "VMagPu", "VMag_kV", "VAngDeg", "VRmsPu", "VRms_kV", "VBase", 
            "islandIdx", "SGen", "SGenTot", "SZip", "SZipTot", "PTot", "PZip", "userData"
        ],
        keysGen: ["id", "componentType", "PTot", "PMin", "PMax", "QMin", "QMax", "phases", "userData"],
        keysZip: ["id", "componentType", "PTot", "phases", "userData"]
    };

    var dom = {
        selector: $("#select-network"),
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
   
    dom.labelCanvas[0].setAttribute("width", dom.labelCanvas[0].offsetWidth);
    dom.labelCanvas[0].setAttribute("height",dom.labelCanvas[0].offsetHeight);
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

    function toGraphCoords(pos) {
        return {
            x: (pos.x - toGraphCoords.pos0.x) * toGraphCoords.scaleFactor, 
            y: -(pos.y - toGraphCoords.pos0.y) * toGraphCoords.scaleFactor};
    }
    toGraphCoords.scaleFactor = 1;
    toGraphCoords.pos0 = {x: 0, y: 0};

    function replacer(key, val) {
        return val && val.toPrecision 
            ? Number(val.toPrecision(6))
            : val;
    }

    function sortedKeys(json, keys) {
        var unsortedKeys = [];
        for (var key in json) {
            if (json.hasOwnProperty(key)) {
                if (keys.indexOf(key) == -1) {
                    unsortedKeys.push(key);
                }
            }
        }

        var result = [];

        for (var i = 0; i < keys.length; ++i) {
            result.push(keys[i]);
        }

        for (var i = 0; i < unsortedKeys.length; ++i) {
            result.push(unsortedKeys[i]);
        }

        return result;
    }

    function loadNetwork(id) {
        removeGraph();
        jQuery.getJSON(params.url + "?view=network", networkLoaded);
    }

    function networkLoaded(netw) {
        var buses = netw.buses;
        var branches = netw.branches;
        var gens = netw.gens;
        var zips = netw.zips;
        
        var busMap = {};
        for (var i = 0; i < buses.length; ++i) {
            var bus = buses[i];
            busMap[bus.id] = bus;
        }

        var genMap = {};
        for (var i = 0; i < gens.length; ++i) {
            var gen = gens[i];
            genMap[gen.id] = gen;
        }
        
        var zipMap = {};
        for (var i = 0; i < zips.length; ++i) {
            var zip = zips[i];
            zipMap[zip.id] = zip;
        }

        // Scale so that average link length is 50 pixels.
        var ls = [];
        for (var i = 0; i < branches.length; ++i) {
            var branch = branches[i];
            var bus0 = busMap[branch.bus0];
            var bus1 = busMap[branch.bus1];
            dx = bus0.coords[0] - bus1.coords[0];
            dy = bus0.coords[1] - bus1.coords[1];
            var l = Math.sqrt(dx * dx + dy * dy);
            ls.push(l);
        }

        function median(xs) {
            if (xs.length == 0) {
                return 0;
            } else if (xs.length == 1) {
                return xs[0];
            } else {
                xs.sort(function(a, b){return a - b;});
                var middle = Math.ceil((xs.length - 1) / 2);
                return ((xs.length - 1) % 2 == 0)
                    ? 0.5 * (xs[middle - 1] + xs[middle])
                    : xs[middle];
            }
        }

        var medLs = median(ls);
        var hasPriorLayout = (medLs > 1e-10);

        if (hasPriorLayout) {
            toGraphCoords.scaleFactor = params.nominalEdgeLength / medLs;

            // Shift to origin is center of graph.
            var maxX = -1e10;
            var minX = 1e10;
            var maxY = -1e10;
            var minY = 1e10;
            for (var i = 0; i < buses.length; ++i) {
                var bus = buses[i];
                if (bus.coords[0] > maxX) maxX = bus.coords[0];
                if (bus.coords[0] < minX) minX = bus.coords[0];
                if (bus.coords[1] > maxY) maxY = bus.coords[1];
                if (bus.coords[1] < minY) minY = bus.coords[1];
            }
            var x0 = 0.5 * (minX + maxX);
            var y0 = 0.5 * (minY + maxY);
            toGraphCoords.pos0 = {x: x0, y: y0};
        }
        
        graph = Viva.Graph.graph();

        for (var i = 0; i < buses.length; ++i) {
            var bus = buses[i];
            var pos = toGraphCoords(
                {x: bus.coords[0], y: bus.coords[1]});

            graph.addNode(bus.id, {
                islandIdx: bus.islandIdx,
                nodeType: "bus",
                pos: pos,
                type: bus.type, 
                VRmsPu: bus.VRmsPu,
                VError: bus.VError
            });

            if (params.usePinNodes) {
                var pinId = "__PIN__" + bus.id;
                graph.addNode(pinId, {
                    nodeType: "pin",
                    pos: pos,
                });

                graph.addLink(bus.id, pinId, {
                    linkType: "pin",
                });
            }
            
            var genIds = bus.gens;
            var nGens = genIds.length;
            for (var j = 0; j < nGens; ++j) {
                var genId = genIds[j];
                var gen = genMap[genId];
                graph.addNode(gen.id, {
                    nodeType: "gen",
                    pos: pos
                });
                graph.addLink(bus.id, gen.id, {
                    linkType: "gen",
                    isInService: gen.isInService
                });
            }

            var zipIds = bus.zips;
            var nZips = zipIds.length;
            for (var j = 0; j < nZips; ++j) {
                var zipId = zipIds[j];
                var zip = zipMap[zipId];
                graph.addNode(zip.id, {
                    nodeType: "zip",
                    pos: pos
                });
                graph.addLink(bus.id, zip.id, {
                    linkType: "zip",
                    isInService: zip.isInService
                });
            }
        }

        for (var i = 0; i < branches.length; ++i) {
            var branch = branches[i];
            graph.addLink(branch.bus0, branch.bus1, {
                id: branch.id,
                linkType: "branch",
                componentType: branch.componentType,
                isInService: branch.isInService
            });
        }
        
        var forceParams = {
                springLength : 0,
                springCoeff : 0.001,
                dragCoeff : 0.05,
                gravity : -1.2
        };

        if (params.usePinNodes) {
            forceParams.springTransform = function (link, spring) {
                if (link.data.linkType == 'branch') {
                    spring.length = 0.0;
                    spring.coeff = 0;
                } else if (link.data.linkType == 'gen') {
                    spring.length = params.busSz + params.genSz + 7;
                    spring.coeff = 0.002;
                } else if (link.data.linkType == 'zip') {
                    spring.length = params.busSz + params.zipSz + 7;
                    spring.coeff = 0.002;
                } else if (link.data.linkType == 'pin') {
                    spring.length = 0.0;
                    spring.coeff = 0.0005;
                }
            };
        } else {
            forceParams.gravity = -5;
            forceParams.dragCoeff = 0.01;
            forceParams.springTransform = function (link, spring) {
                if (link.data.linkType == 'branch') {
                    spring.length = 6 * params.busSz;
                } else if (link.data.linkType == 'gen') {
                    spring.length = params.busSz + params.genSz + 7;
                    spring.coeff = 0.001;
                } else if (link.data.linkType == 'zip') {
                    spring.length = params.busSz + params.zipSz + 7;
                    spring.coeff = 0.001;
                }
            };
        }

        layout = Viva.Graph.Layout.forceDirected(graph, forceParams);

        // Make sure pins don't repel their nodes.
        // TODO: is this right?
        graph.forEachNode(function(node) {
            var body = layout.getBody(node.id);
            if (node.data.nodeType == "pin") {
                body.mass = 0.0;
            }
        });

        graphics = Viva.Graph.View.svgGraphics();

        graphics.node(function(node) {
            var nodeType = node.data.nodeType;
            if (nodeType == "pin") return Viva.Graph.svg("svg");

            var someColors = [
                "Coral", "Cyan", "DarkGray", "HotPink", "DarkSalmon", "Khaki", "Indigo", "BlueViolet", "Brown", "Purple", "PowderBlue", "DarkBlue", "Chocolate", "DarkMagenta", "Black", "Orange", "Gold", "GreenYellow", "Plum", "Olive", "LightBlue", "Lime", "Maroon", "MintCream", "MistyRose", "Moccasin", "Navy", "OldLace", "OliveDrab", "OrangeRed", "Orchid", "PaleGoldenRod", "PaleGreen", "PaleTurquoise", "PaleVioletRed", "AliceBlue", "Aqua", "Aquamarine", "Azure", "Bisque", "BlanchedAlmond", "Blue", "BurlyWood", "CadetBlue", "Chartreuse", "CornflowerBlue", "Cornsilk", "Crimson", "DarkCyan", "DarkGoldenRod", "DarkGrey", "DarkGreen", "DarkKhaki", "DarkOliveGreen", "Darkorange", "DarkOrchid", "DarkRed", "DarkSeaGreen", "DarkSlateBlue", "DarkSlateGray", "DarkSlateGrey", "DarkTurquoise", "DarkViolet", "DeepPink", "DeepSkyBlue", "DimGray", "DimGrey", "DodgerBlue", "FireBrick", "ForestGreen", "Fuchsia", "Gainsboro", "GoldenRod", "Gray", "Grey", "Green", "HoneyDew", "IndianRed", "Ivory", "Lavender", "LavenderBlush", "LawnGreen", "LemonChiffon", "LightCoral", "LightCyan", "LightGoldenRodYellow", "LightGray", "LightGreen", "LightPink", "LightSalmon", "LightSeaGreen", "LightSkyBlue", "LightSlateGray", "LightSlateGrey", "LightSteelBlue", "LightYellow", "LimeGreen", "Linen", "Magenta", "MediumAquaMarine", "MediumBlue", "MediumOrchid", "MediumPurple", "MediumSeaGreen", "MediumSlateBlue", "MediumSpringGreen", "MediumTurquoise", "MediumVioletRed", "MidnightBlue", "PapayaWhip", "PeachPuff", "Peru", "Pink", "Red", "RosyBrown", "RoyalBlue", "SaddleBrown", "Salmon", "SandyBrown", "SeaGreen", "SeaShell", "Sienna", "Silver", "SkyBlue", "SlateBlue", "SlateGray", "SlateGrey", "Snow", "SpringGreen", "SteelBlue", "Tan", "Teal", "Thistle", "Tomato", "Turquoise", "Violet", "Wheat", "Yellow", "YellowGreen"]; 

            var sz;
            var color;
            var selector;
            var keys;

            if (nodeType == "bus") {
                sz = params.busSz;
                color = someColors[node.data.islandIdx % someColors.length];
                keys = params.keysBus;
                selector = "?view=bus&id=";
            } else if (nodeType == "gen") {
                sz = params.genSz;
                color = "Red";
                keys = params.keysGen;
                selector = "?view=gen&id=";
            } else if (nodeType == "zip") {
                sz = params.zipSz;
                color = "Green";
                keys = params.keysZip;
                selector = "?view=zip&id=";
            }
            var ui = Viva.Graph.svg("circle")
                .attr("r", sz)
                .attr("fill", color);
            $(ui).click(function() {
                var url = params.url + selector + node.id;
                $.ajax({
                    url: url,
                    async: false,
                    dataType: "json",
                    success: function (response) {
                        var opt = { 
                            change: function(data) {},
                            propertyclick: function(path) {},
                            keys: keys,
                            replacer: replacer
                        };
                        dom.properties.jsonEditor(response, opt);
                    }
                });
            });
            return ui;
        });

        graphics.placeNode(function(nodeUI, pos) {
            nodeUI
                .attr("cx", pos.x)
                .attr("cy", pos.y)
                .attr("style", "stroke: white; stroke-width: 3px;");
        });
        
        graphics.link(function(link) {
            var linkType = link.data.linkType;
            if (linkType == "pin") return Viva.Graph.svg("svg");

            var ui = Viva.Graph.svg("line");

            if (linkType == "branch") {
                if (link.data.componentType.indexOf("trans") > -1)  {
                    ui.attr("stroke", "red");
                } else {
                    ui.attr("stroke", "#2F4F4F");
                }
            } else if (linkType == "gen" || linkType == "zip") {
                ui.attr("stroke", "#568F8F");
            }

            ui.attr("stroke-width", "6px");

            if (!link.data.isInService) {
                ui.attr("stroke-dasharray", [4,4]);
            }
            
            $(ui).click(function() {
                var url = params.url + "?view=branch&id=" + link.data["id"];
                $.ajax({
                    url: url,
                    async: false,
                    dataType: "json",
                    success: function (response) {
                        var keys = sortedKeys(response, params.keysBranch);
                        var opt = { 
                            change: function(data) {},
                            propertyclick: function(path) {},
                            keys: keys,
                            replacer: replacer
                        };
                        dom.properties.jsonEditor(response, opt);

                    }
                });
            });
            return ui;
        });

        renderer = Viva.Graph.View.renderer(graph, {
            layout   : layout,
            graphics   : graphics,
            renderLinks : true,
            prerender  : true,
            container  : document.getElementById("sgt-network-graph")
        });

        oldEndRender = graphics.endRender;
        graphics.endRender = function() {
            oldEndRender();
            if (dom.showHeatmap[0].checked) drawHeatmap();
        }; 
       
        if (params.usePinNodes) {
            // Pin buses.
            graph.forEachNode(function(node) {
                layout.setNodePosition(node.id, node.data.pos.x, node.data.pos.y);
                if (hasPriorLayout && 
                    ((!params.usePinNodes && node.data.nodeType == "bus") ||
                     node.data.nodeType == "pin"))
                     {
                         layout.pinNode(node, true);
                     }
            });
        }
        
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
        "http://sgt.com/api?view=input_files&suffix=m",
        function(files) {
            for (var i = 0; i < files.length; ++i) {
                dom.selector.append("<option>" + files[i] + "</option>");
            }
        }
    );

    dom.selector.change(
        function() {
            var file = $(this).find("option:selected").text();
            var url = "http://sgt.com/api?action=load_matpower&file=" + file;
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
        var svg = $("g")[0];
        var t = svg.getCTM();
        graph.forEachNode(function(node) {
            if (node.data.nodeType != "bus") {
                return;
            }
            var pos = layout.getNodePosition(node.id);
            var newPos = {x: t.a * pos.x + t.b * pos.y + t.e, y: t.c * pos.x
                + t.d * pos.y + t.f};
            // var dV = 1.0 + 10 * node.data.VError;
            // var VParam = (clamp(dV, VLow, VHigh) - VLow) / VRange;
            var VParam = 
                (clamp(node.data.VRmsPu, VLow, VHigh) - VLow) / VRange;
            dat.push([[newPos.x, newPos.y], VParam]);
        });
        Dexter.Heatmap.setData(dat);
        Dexter.Heatmap.draw(); // adds the buffered points
    }

    function clearLabels()
    {
        labelCtx.clearRect(0, 0, dom.labelCanvas[0].scrollWidth,
                           dom.labelCanvas[0].scrollHeight);
    }

    function drawLabels()
    {
        clearLabels();
        graph.forEachNode(function(node) {
            var pos = layout.getNodePosition(node.id);
            var newPos = graphics.transformGraphToClientCoordinates(
                {x: pos.x, y: pos.y + 15});
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
        VLow = dom.rangeVLow[0].value / 1000;
        VHigh = dom.rangeVHigh[0].value / 1000;
        if (VLow > VHigh) {
            VLow = VHigh;
            dom.rangeVLow[0].value = 1000 * VLow;
        }
        dom.labelVLow[0].innerHTML = VLow;
        VRange = VHigh - VLow;
        if (dom.showHeatmap[0].checked) drawHeatmap();
    }

    function syncVHigh() {
        VLow = dom.rangeVLow[0].value / 1000;
        VHigh = dom.rangeVHigh[0].value / 1000;
        if (VHigh < VLow) {
            VHigh = VLow;
            dom.rangeVHigh[0].value = 1000 * VHigh;
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
        var iterPercent = 
            100 * (params.iMaxPrecompute - iter) / params.iMaxPrecompute;
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
