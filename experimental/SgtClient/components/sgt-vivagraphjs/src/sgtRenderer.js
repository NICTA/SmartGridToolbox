var eventify = require('ngraph.events');
var forceDirected = require('ngraph.forcelayout');
var svgGraphics = require('../../vivagraphjs/src/View/svgGraphics.js');
var domInputManager = require('../../vivagraphjs/src/Input/domInputManager.js');
var timer = require('../../vivagraphjs/src/Utils/timer.js');

function sgtRenderer(graph, settings) {
    var FRAME_INTERVAL = 30;

    settings = settings || {};

    var layout = settings.layout,
        graphics = settings.graphics,
        map = settings.map,
        container = map.getPanes().overlayPane,
        inputManager,
        animationTimer,
        rendererInitialized = false,

        currentStep = 0,
        totalIterationsCount = 0,
        isStable = false,
        userInteractionInProgress = false,
        isPaused = false,

        publicEvents = eventify({}),
        containerDrag;

    return {
        run: function(iterationsCount) {

            if (!rendererInitialized) {
                prepareSettings();
                prerender();

                initDom();
                listenToEvents();

                rendererInitialized = true;
            }

            renderIterations(iterationsCount);

            return this;
        },

        pause: function() {
            isPaused = true;
            animationTimer.stop();
        },

        resume: function() {
            isPaused = false;
            animationTimer.restart();
        },

        rerender: function() {
            renderGraph();
            return this;
        },

        getGraphics: function() {
            return graphics;
        },

        dispose: function() {
            stopListenToEvents(); // I quit!
        },

        on: function(eventName, callback) {
            publicEvents.on(eventName, callback);
            return this;
        },

        off: function(eventName, callback) {
            publicEvents.off(eventName, callback);
            return this;
        }
    };

    function prepareSettings() {
        layout = layout || forceDirected(graph, {springLength: 80, springCoeff: 0.0002});
        graphics = graphics || svgGraphics(graph, {container: container});

        settings.prerender = settings.prerender || 0;
        inputManager = (graphics.inputManager || domInputManager)(graph, graphics);
    }

    function renderGraph() {
        graphics.beginRender();

        graphics.renderLinks();
        graphics.renderNodes();
        graphics.endRender();
    }

    function onRenderFrame() {
        isStable = layout.step() && !userInteractionInProgress;
        renderGraph();

        return !isStable;
    }

    function renderIterations(iterationsCount) {
        if (animationTimer) {
            totalIterationsCount += iterationsCount;
            return;
        }

        if (iterationsCount) {
            totalIterationsCount += iterationsCount;

            animationTimer = timer(function() {
                return onRenderFrame();
            }, FRAME_INTERVAL);
        } else {
            currentStep = 0;
            totalIterationsCount = 0;
            animationTimer = timer(onRenderFrame, FRAME_INTERVAL);
        }
    }

    function resetStable() {
        if (isPaused) {
            return;
        }

        isStable = false;
        animationTimer.restart();
    }

    function prerender() {
        if (typeof settings.prerender === 'number' && settings.prerender > 0) {
            for (var i = 0; i < settings.prerender; i += 1) {
                layout.step();
            }
        }
    }

    function createNodeUi(node) {
        var nodePosition = layout.getNodePosition(node.id);
        graphics.addNode(node, nodePosition);
    }

    function removeNodeUi(node) {
        graphics.releaseNode(node);
    }

    function createLinkUi(link) {
        var linkPosition = layout.getLinkPosition(link.id);
        graphics.addLink(link, linkPosition);
    }

    function removeLinkUi(link) {
        graphics.releaseLink(link);
    }

    function listenNodeEvents(node) {
        var wasPinned = false;

        // TODO: This may not be memory efficient. Consider reusing handlers object.
        inputManager.bindDragNDrop(node, {
            onStart: function() {
                wasPinned = layout.isNodePinned(node);
                layout.pinNode(node, true);
                userInteractionInProgress = true;
                resetStable();
            },
            onDrag: function(e, offset) {
                var svg = container.childNodes[0].childNodes[0];
                var ctm = svg.getScreenCTM();
                var scale = ctm.a;
                var scaledOffset = {x: offset.x / scale, y: offset.y / scale};

                var oldPos = layout.getNodePosition(node.id);
                layout.setNodePosition(node.id, oldPos.x + scaledOffset.x, oldPos.y + scaledOffset.y);

                userInteractionInProgress = true;
                renderGraph();
            },
            onStop: function() {
                layout.pinNode(node, wasPinned);
                userInteractionInProgress = false;
            }
        });
    }

    function releaseNodeEvents(node) {
        inputManager.bindDragNDrop(node, null);
    }

    function initDom() {
        graphics.init(container);
        graph.forEachNode(createNodeUi);
        graph.forEachLink(createLinkUi);
    }

    function releaseDom() {
        graphics.release(container);
    }

    function processNodeChange(change) {
        var node = change.node;

        if (change.changeType === 'add') {
            createNodeUi(node);
            listenNodeEvents(node);
        } else if (change.changeType === 'remove') {
            releaseNodeEvents(node);
            removeNodeUi(node);
        } else if (change.changeType === 'update') {
            releaseNodeEvents(node);
            removeNodeUi(node);

            createNodeUi(node);
            listenNodeEvents(node);
        }
    }

    function processLinkChange(change) {
        var link = change.link;
        if (change.changeType === 'add') {
            createLinkUi(link);
        } else if (change.changeType === 'remove') {
            removeLinkUi(link);
        } else if (change.changeType === 'update') {
            throw 'Update type is not implemented. TODO: Implement me!';
        }
    }

    function onGraphChanged(changes) {
        var i, change;
        for (i = 0; i < changes.length; i += 1) {
            change = changes[i];
            if (change.node) {
                processNodeChange(change);
            } else if (change.link) {
                processLinkChange(change);
            }
        }

        resetStable();
    }

    function releaseContainerDragManager() {
        if (containerDrag) {
            containerDrag.release();
            containerDrag = null;
        }
    }

    function releaseGraphEvents() {
        graph.off('changed', onGraphChanged);
    }

    function listenToEvents() {
        graph.forEachNode(listenNodeEvents);
        releaseGraphEvents();
        graph.on('changed', onGraphChanged);
    }

    function stopListenToEvents() {
        rendererInitialized = false;
        releaseGraphEvents();
        publicEvents.off();
        animationTimer.stop();

        graph.forEachLink(function(link) {
            removeLinkUi(link);
        });

        graph.forEachNode(function(node) {
            releaseNodeEvents(node);
            removeNodeUi(node);
        });

        layout.dispose();
        releaseDom();
    }
}

module.exports = sgtRenderer;
