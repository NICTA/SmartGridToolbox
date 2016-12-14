var Sgt = Sgt || {};

Sgt.SgtClient = (function() {
    // Parameters:
    var params = {
        iMaxPrecompute: 500,
        tMaxPrecompute: 1000,
        nominalEdgeLength: 50,
        busSz: 20,
        genSz: 50,
        zipSz: 10,
        genSeparator: 0,
        zipSeparator: -6,
        usePinNodes: true,
        initZoom: 12,
        datumRef: {x: 526711.07, y: 5249625.32},
        latLongRef: {lat: -42.905518, lon: 147.327209},
        url: "http://sgt.com/api/",
        keysBranch: ["id", "componentType", "phases0", "phases1", "Y", "userData"],
        keysBus: [
            "id", "componentType", "phases", "V", "VSincal", "VMagPu", "VMagPuSincal", "VMag_kV", "VMag_kV_Sincal", 
            "VAngDeg", "VAngDegSincal", "VRmsPu", "VRmsPuSincal", "VRms_kV", "VRms_kV_Sincal", "VError", "VBase", 
            "islandIdx", "SGen", "SGenTot", "SZip", "SZipTot", "PTot", "PZip", "userData"
        ],
        keysGen: ["id", "componentType", "PTot", "userData"],
        keysZip: ["id", "componentType", "PTot", "phases", "userData"]
    };

    var cEarth = 2 * Math.PI * 6371000;
    var cLat = cEarth * Math.cos(Math.PI * params.latLongRef.lat / 180)
    var scaleFactor = null;
    var pos0 = null;
    var initTableTransform = null;
    var simPlaying = false;
    
    var buses = [];
    var branches = [];
    var gens = [];
    var zips = [];

    // Create map:
    var map = L.map("sgt-map")

    // JQuery elements for DOM:
    var dom = {
        map: $("#sgt-map"),
        overlay: $(".leaflet-overlay-pane"),
        useSpring: $("#use-spring-layout"),
        showHeatmap: $("#show-heatmap"),
        rangeVLow: $("#range-V-low"),
        rangeVHigh: $("#range-V-high"),
        labelVLow: $("#label-V-low"),
        labelVHigh: $("#label-V-high"),
        heatmapCanvas: $("#sgt-heatmap-canvas"),
        properties: $("#sgt-network-properties"),
        progressGroup: $("#sgt-progress-group"),
        progressMessage: $("#sgt-progress-message"),
        progress: $("#sgt-progress"),
        svg: null,
        table: null
    };

    // Map layer:
    map.setView([-43.395374, 147.281696], params.initZoom);
    L.tileLayer("http://{s}.tile.osm.org/{z}/{x}/{y}.png",
        {
            attribution: "&copy; <a href='http://osm.org/copyright'>OpenStreetMap</a> contributors", opacity: 0.6
        }).addTo(map);
    map.networkOrigin = map.layerPointToLatLng([0.0, 0.0]);
    
    map.on("dragstart", function() {
        dom.heatmapCanvas[0].setAttribute("hidden");
    });
    
    map.on("zoomstart", function() {
        dom.heatmapCanvas[0].setAttribute("hidden");
    });

    map.on("dragend", function() {
        positionSvg();
        positionTable();
        syncHeatmap();
        drawHeatmap();
    });

    map.on("zoomend", function() {
        positionSvg();
        positionTable();
        syncHeatmap();
        drawHeatmap();
    });

    map.on("resize", function() {
        positionSvg();
        positionTable();
        syncHeatmap();
        drawHeatmap();
    });
        
    // Vivagraph elements:
    var graph = null;
    var layout = null;
    var graphics = null;
    var renderer = null;

    // Low and high voltage variables:
    var VLow = 0.0;
    var VHigh = 2.0;
    var VRange;
    syncVLow();
    syncVHigh();

    
    // Heatmap:
    window.addEventListener("resize", function () {Dexter.Heatmap.setViewRectToCanvas();});
    Dexter.Heatmap.init(dom.heatmapCanvas[0]);
    Dexter.Heatmap.setViewRectToCanvas();
    function loadNetwork() {
        removeGraph();
        // showProgress(true, "Loading network " + id + ". Please wait.");
        jQuery.getJSON(params.url + "?view=network", 
            function(netw) {
                buses = netw.buses;
                branches = netw.branches;
                gens = netw.gens;
                zips = netw.zips;

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

                var medLs = median(ls);
                var hasPriorLayout = (medLs > 1e-10);

                if (hasPriorLayout) {
                    // Set up transform of datum coords to pixel coords, so origin is center of graph, and the median
                    // edge length equals the nominal edge length (e.g. 50 pixels).
                    scaleFactor = params.nominalEdgeLength / medLs;
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
                    pos0 = {x: x0, y: y0};
                }

                graph = Viva.Graph.graph();

                // Add SVGs in order from back to front.
                
                // Step 1: add all gens and zips.
                for (var i = 0; i < buses.length; ++i) {
                    var bus = buses[i];
                    var pos = datumToGraph({x: bus.coords[0], y: bus.coords[1]});

                    var genIds = bus.gens;
                    var nGens = genIds.length;
                    for (var j = 0; j < nGens; ++j) {
                        var genId = genIds[j];
                        var gen = genMap[genId];
                        graph.addNode(gen.id, {
                            nodeType: "gen",
                            pos: pos,
                            PTot: gen.PTot
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
                            pos: pos,
                            PTot: zip.PTot
                        });
                        graph.addLink(bus.id, zip.id, {
                            linkType: "zip",
                            isInService: zip.isInService
                        });
                    }
                }
                
                // Step 2: add all buses.
                for (var i = 0; i < buses.length; ++i) {
                    var bus = buses[i];
                    var pos = datumToGraph({x: bus.coords[0], y: bus.coords[1]});

                    graph.addNode(bus.id, {
                        islandIdx: bus.islandIdx,
                        nodeType: "bus",
                        pos: pos,
                        type: bus.type,
                        VRmsPu: bus.VRmsPu,
                        PTot: bus.PTot,
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
                };

                // Step 3: add all branches.
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
                    springLength : 0.0,
                    springCoeff : 0.0,
                    dragCoeff : 0.05,
                    gravity : -5
                };
                if (params.usePinNodes) {
                    forceParams.springTransform = function (link, spring) {
                        if (link.data.linkType == "branch") {
                            spring.coeff = 0;
                        } else if (link.data.linkType == "gen") {
                            spring.length = params.busSz + params.genSz + params.genSeparator;
                            spring.coeff = 0.002;
                        } else if (link.data.linkType == "zip") {
                            spring.length = params.busSz + params.zipSz + params.zipSeparator;
                            spring.coeff = 0.002;
                        } else if (link.data.linkType == "pin") {
                            spring.length = 0.0;
                            spring.coeff = 0.0005;
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

                graphics = getGraphics();

                renderer = Viva.Graph.View.renderer(graph, {
                    layout: layout,
                    graphics: graphics,
                    renderLinks: true,
                    prerender: true,
                    container: map.getPanes().overlayPane,
                    interactive: "node"
                });
                renderer.off("scroll");

                oldEndRender = graphics.endRender;
                graphics.endRender = function() {
                    oldEndRender();
                    if (dom.showHeatmap[0].checked) drawHeatmap();
                };

                // Pin buses.
                graph.forEachNode(function(node) {
                    layout.setNodePosition(node.id, node.data.pos.x, node.data.pos.y);
                    if (hasPriorLayout && ((!params.usePinNodes && node.data.nodeType == "bus") ||
                        node.data.nodeType == "pin"))
                    {
                        layout.pinNode(node, true);
                    }
                });

                // we need to compute layout, but we don"t want to freeze the browser
                var tStart = new Date();
                var t = 0;
                showProgress(true, "Laying out network graph, please wait.");
                precompute(params.iMaxPrecompute, 
                    function() {
                        renderer.run(0); // We need this to make sure the SVG elements get initialized.
                        dom.svg = $("div > svg");
                        dom.table = $("svg > g"); 
                        dom.table.addClass("leaflet-zoom-hide");
                        dom.svg.css("width", dom.map.width()); 
                        dom.svg.css("height", dom.map.height());

                        setInitSvgBox(dom.svg);
                        
                        renderer.run();
                    });

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
        );
    }


    function getGraphics()
    {
        var graphics = Viva.Graph.View.svgGraphics();

        graphics.node(function(node) {
            var nodeType = node.data.nodeType;
            var ui;
            if (nodeType == "pin") 
            {
                ui = Viva.Graph.svg("svg");
            }
            else
            {
                var someColors = [
                    "Coral", "Cyan", "DarkGray", "HotPink", "DarkSalmon", "Khaki", "Indigo", "BlueViolet", "Brown",
                    "Purple", "PowderBlue", "DarkBlue", "Chocolate", "DarkMagenta", "Black", "Orange", "Gold",
                    "GreenYellow", "Plum", "Olive", "LightBlue", "Lime", "Maroon", "MintCream", "MistyRose",
                    "Moccasin", "Navy", "OldLace", "OliveDrab", "OrangeRed", "Orchid", "PaleGoldenRod", "PaleGreen",
                    "PaleTurquoise", "PaleVioletRed", "AliceBlue", "Aqua", "Aquamarine", "Azure", "Bisque",
                    "BlanchedAlmond", "Blue", "BurlyWood", "CadetBlue", "Chartreuse", "CornflowerBlue", "Cornsilk",
                    "Crimson", "DarkCyan", "DarkGoldenRod", "DarkGrey", "DarkGreen", "DarkKhaki", "DarkOliveGreen",
                    "Darkorange", "DarkOrchid", "DarkRed", "DarkSeaGreen", "DarkSlateBlue", "DarkSlateGray",
                    "DarkSlateGrey", "DarkTurquoise", "DarkViolet", "DeepPink", "DeepSkyBlue", "DimGray", "DimGrey",
                    "DodgerBlue", "FireBrick", "ForestGreen", "Fuchsia", "Gainsboro", "GoldenRod", "Gray", "Grey",
                    "Green", "HoneyDew", "IndianRed", "Ivory", "Lavender", "LavenderBlush", "LawnGreen",
                    "LemonChiffon", "LightCoral", "LightCyan", "LightGoldenRodYellow", "LightGray", "LightGreen",
                    "LightPink", "LightSalmon", "LightSeaGreen", "LightSkyBlue", "LightSlateGray", "LightSlateGrey",
                    "LightSteelBlue", "LightYellow", "LimeGreen", "Linen", "Magenta", "MediumAquaMarine",
                    "MediumBlue", "MediumOrchid", "MediumPurple", "MediumSeaGreen", "MediumSlateBlue",
                    "MediumSpringGreen", "MediumTurquoise", "MediumVioletRed", "MidnightBlue", "PapayaWhip",
                    "PeachPuff", "Peru", "Pink", "Red", "RosyBrown", "RoyalBlue", "SaddleBrown", "Salmon",
                    "SandyBrown", "SeaGreen", "SeaShell", "Sienna", "Silver", "SkyBlue", "SlateBlue", "SlateGray",
                    "SlateGrey", "Snow", "SpringGreen", "SteelBlue", "Tan", "Teal", "Thistle", "Tomato", "Turquoise",
                    "Violet", "Wheat", "Yellow", "YellowGreen"];

                var query;
                var keys;
                var ui = Viva.Graph.svg("g");

                if (nodeType == "bus") {
                    query = "?view=bus&id=" + node.id;
                    keys = params.keysBus;

                    var color = someColors[node.data.islandIdx % someColors.length];
                    var circ = Viva.Graph.svg("circle")
                        .attr("r", params.busSz).attr("fill", color)
                        .attr("style", "stroke: white; stroke-width: 6px;");;
                    ui.append(circ);
                } else if (nodeType == "gen") {
                    query = "?view=gen&id=" + node.id;
                    keys = params.keysGen;

                    var circ = Viva.Graph.svg("circle")
                        .attr("r", params.genSz).attr("fill", "Red")
                        .attr("style", "stroke: white; stroke-width: 6px;");;
                    ui.append(circ);
                } else if (nodeType == "zip") {
                    query = "?view=zip&id=" + node.id;
                    keys = params.keysZip;

                    var PTot = node.data.PTot;
                    outerSz = zipSize(PTot);
                    color = PTot > 0 ? "Blue" : "Red";
                    var outer = Viva.Graph.svg("circle")
                        .attr("r", outerSz).attr("fill", color).attr("opacity", 0.6);
                    ui.append(outer);
                    var inner = Viva.Graph.svg("circle")
                        .attr("r", params.zipSz).attr("fill", color)
                        .attr("style", "stroke: white; stroke-width: 6px;");
                    ui.append(inner);
                }

                $(ui).click(function() {
                    var url = params.url + query;
                    $.ajax({
                        url: url,
                        async: false,
                        dataType: "json",
                        success: function (response) {
                            var opt = { 
                                change: function(data) {},
                                propertyclick: function(path) {},
                                keys: sortedKeys(response, keys),
                                replacer: replacer
                            };
                            dom.properties.jsonEditor(response, opt);
                        }
                    });
                });
            }
            node.data.ui = ui;
            return ui;
        });

        graphics.placeNode(function(nodeUI, pos) {
            var off = 0.5 * nodeUI.getBBox().width;
            nodeUI.attr('transform', 'translate(' + (pos.x) + ',' + (pos.y) + ')');
        });

        graphics.link(function(link) {
            var linkType = link.data.linkType;
            if (linkType == "pin") return Viva.Graph.svg("svg");

            var ui = Viva.Graph.svg("line");

            if (linkType == "branch" && link.data.componentType.indexOf("trans") > -1)  {
                ui.attr("stroke", "red");
            } else {
                ui.attr("stroke", "#2F4F4F");
            }

            ui.attr("stroke-width", "16px");
            if (!link.data.isInService) {
                ui.attr("stroke-dasharray", [4,4]);
            }

            $(ui).click(function() {
                console.log("branch click");
                var url = params.url + "/branches/?view=branch&id=" + link.data["id"];
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

        return graphics;
    }

    function startSim() {
        simPlaying = true;
        stepSim();
    }
    
    function stopSim() {
        simPlaying = false;
    }
           
    function getAndHandleVoltagesAndPowers(msg) {
        jQuery.getJSON(
            params.url + "?view=bus_voltages", function(voltages) {
                jQuery.getJSON(
                    params.url + "?view=zip_powers", function(powers) {
                        simStepped(voltages, powers);
                    }
                )
            }
        )
    }

    function resetSim() {
        $.ajax({
            type: "PUT",
            contentType: "application/json; charset=utf-8",
            url: params.url + "?action=reset",
            data: JSON.stringify({}),
            dataType: "json",
            success: getAndHandleVoltagesAndPowers,
            error: function (err){
                alert("Error resetting simulation.");
            }
        });
    }
    
    function stepSim() {
        $.ajax({
            type: "PUT",
            contentType: "application/json; charset=utf-8",
            url: params.url + "?action=step",
            data: JSON.stringify({}),
            dataType: "json",
            success: getAndHandleVoltagesAndPowers,
            error: function (err){
                alert("Error stepping simulation.");
            }
        });
    }

    function simStepped(voltages, powers) {
        for (var i = 0; i < voltages.length; ++i) {
            var Vi = voltages[i];
            var node = graph.getNode(buses[i].id);
            node.data.VRmsPu = Vi;
        }
        
        for (var i = 0; i < powers.length; ++i) {
            var Pi = powers[i];
            var node = graph.getNode(zips[i].id);
            node.data.PTot = Pi;
        }

        drawHeatmap();
        updateNodes();
        if (simPlaying) {
            stepSim();
        }
    }

    function removeGraph() {
        if (!renderer) {
            return; // already removed
        }
        renderer.dispose(); // remove the graph
        renderer = null;
    }

    function drawHeatmap() {
        dat = [];
        var t = dom.table[0].getCTM();
        graph.forEachNode(function(node) {
            if (node.data.nodeType != "bus") {
                return;
            }
            var pos = layout.getNodePosition(node.id);
            var newPos = {x: t.a * pos.x + t.b * pos.y + t.e, y: t.c * pos.x
                + t.d * pos.y + t.f};
            // var dV = 1.0 + 10 * node.data.VError;
            // var VParam = (clamp(dV, VLow, VHigh) - VLow) / VRange;
            // var VParam = (clamp(-node.data.PTot, -0.005, 0.002) + 0.005) / 0.007;
            var VParam = (clamp(node.data.VRmsPu, VLow, VHigh) - VLow) / VRange;
            dat.push([[newPos.x, newPos.y], VParam]);
        });
        Dexter.Heatmap.setData(dat);
        Dexter.Heatmap.draw(); // adds the buffered points
    }

    function updateNodes() {
        graph.forEachNode(function(node) {
            if (node.data.nodeType == "zip")
            {
                color = node.data.PTot > 0 ? "Blue" : "Red";
                node.data.ui.childNodes[0].attr("r", zipSize(node.data.PTot)).attr("fill", color);
            }
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
        var iterPercent = 100 * (params.iMaxPrecompute - iter) / params.iMaxPrecompute;
        var tPercent = 100 * t / params.tMaxPrecompute;
        var percent = Math.max(iterPercent, tPercent);
        dom.progress[0].value = percent;
    }

    // Transformation functions:

    // Datum (e.g. as supplied in original data) to graph coords, which can be interpreted as pixel coordinates.
    function datumToGraph(pos) {
        return {
            x: (pos.x - pos0.x) * scaleFactor,
            y: -(pos.y - pos0.y) * scaleFactor
        };
    }
    
    function graphToDatum(pos) {
        return {
            x: (pos.x / scaleFactor) + pos0.x,
            y: (pos.y / scaleFactor) + pos0.y
        };
    }

    function datumToLatLong(pos) {
        var dFromRef = {x: pos.x - params.datumRef.x, y: pos.y - params.datumRef.y};
        var degFromRef = {lat: 360 * dFromRef.y / cEarth, lon: 360 * dFromRef.x / cLat};
        var latLong = {lat: params.latLongRef.lat + degFromRef.lat, lon: params.latLongRef.lon + degFromRef.lon};
        return latLong;
    }

    function latLongToDatum(latLong) {
        var degFromRef = {lat: latLong.lat - params.latLongRef.lat, lon: latLong.lon - params.latLongRef.lon};
        var dFromRef = {
            x: degFromRef.lon * cLat / 360, 
            y: degFromRef.lat * cEarth / 360
        };
        var pos = {x: params.datumRef.x + dFromRef.x, y: params.datumRef.y + dFromRef.y} 
        return pos;
    }

    function graphToLatLong(pos)
    {
        return datumToLatLong(graphToDatum(pos));
    }

    function latLongToGraph(latLong)
    {
        return datumToGraph(latLongToDatum(latLong));
    }

    function positionTable() {
        var scale = map.getZoomScale(map.getZoom(), params.initZoom); // Factor from initial zoom.
		var point = map.latLngToContainerPoint(map.networkOrigin);
        dom.table.attr(
            "transform",
            "translate(" + point.x + "," + point.y + ")scale(" + scale + ")" + initTableTransform);
    }

    function positionSvg() {
        var point = map.containerPointToLayerPoint([0,0]);
		dom.svg.css("left", point.x + "px").css("top", point.y + "px");
    }

    function setInitSvgBox() {
        var mapBounds = map.getBounds();
        var mapTl = latLongToGraph({lat: mapBounds.getNorth(), lon: mapBounds.getWest()});
        var mapBr = latLongToGraph({lat: mapBounds.getSouth(), lon: mapBounds.getEast()});
        var mapDx = mapBr.x - mapTl.x;

        // Scale so that width -> mapDx: 
        var scale = dom.map.width() / mapDx;
        initTableTransform = "scale(" + scale + ")translate(" + -mapTl.x + "," + -mapTl.y + ")";
        dom.table.attr("transform", initTableTransform);
    }

    function zipSize(PTot) {
        return 20 + 200 * params.zipSz * Math.abs(PTot);
    }

    // Utility functions:
    function clamp(x, min, max) {
        return Math.max(min, Math.min(x, max));
    }

    function median(xs) {
        if (xs.length == 0) {
            return 0;
        } else if (xs.length == 1) {
            return xs[0];
        } else {
            xs.sort(function(a, b){return a - b;});
            var middle = Math.ceil((xs.length - 1) / 2);
            return ((xs.length - 1) % 2 == 0) ? 0.5 * (xs[middle - 1] + xs[middle]) : xs[middle];
        }
    }

    function replacer(key, val) {
        return val && val.toPrecision ? Number(val.toPrecision(6)) : val;
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

    return {
        "loadNetwork": loadNetwork,
        "startSim": startSim,
        "stopSim": stopSim,
        "resetSim": resetSim,
        "syncSpringLayout": syncSpringLayout,
        "syncHeatmap": syncHeatmap,
        "syncVLow": syncVLow,
        "syncVHigh": syncVHigh
    };
}());

$( document ).ready(function() {
    Sgt.SgtClient.loadNetwork();
});
