var N = 64;
var nTri = 2 * (N - 1) * (N - 1);
var nInd = 3 * nTri;

var canvas;
var gl;
var verticesBuffer;
var vertexIndicesBuffer;
var verticesColorBuffer;
var vertexPositionAttribute;
var shaderProgram;

var dat;

//
// start
//
// Called when the canvas is created to get the ball rolling.
//
function start() {
    canvas = document.getElementById("glcanvas");

    initWebGL(canvas);      // Initialize the GL context

    // Only continue if WebGL is available and working

    if (gl) {
        gl.enable(gl.BLEND);
        gl.blendEquation(gl.FUNC_ADD);
        gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);
        gl.clearColor(1, 1, 1, 1);  // Clear to black, fully opaque
        gl.clearDepth(1.0);                 // Clear everything

        initShaders();
        initData();
        initBuffers();
        drawScene();
    }
}

function initWebGL() {
    gl = null;

    try {
        gl = canvas.getContext("experimental-webgl");
    }
    catch(e) {
    }

    if (!gl) {
        alert("Unable to initialize WebGL. Your browser may not support it.");
    }
}

function initData() {
    function rand(min, max)
    {
        return min + Math.random() * (max - min);
    }

    var nDat = 10;
    dat = [];
    for (var i = 0; i < nDat; ++i)
    {
        dat.push([rand(-1, 1), rand(-1, 1), rand(0, 1)]);
    }
}

function initBuffers() {
    var spreadVal = 0.1;
    var spreadSat = 0.1;

    function idx(i, j) {
        return i * N + j;
    }
    
    function disp(p1, p2) {
        return [p1[0] - p2[0], p1[1] - p2[1]];
    }
    
    function manhattan(disp) {
        return Math.max(Math.abs(disp[0]), Math.abs(disp[1]));
    }

    function weight(d, s)
    {
        var dds = [d[0] / s, d[1] / s];
        var l2 = dds[0] * dds[0] + dds[1] * dds[1];
        return 1.0 / (1.0 + l2); // Cauchy distribution.
    }

    var vertices = [];
    var vertColorWork = [];
    var verticesColor = [];
    for (var i = 0; i < N; ++i) {
        var x = 2 * i / (N - 1) - 1;
        for (var j = 0; j < N; ++j) {
            var y = 2 * j / (N - 1) - 1;
            vertices.push.apply(vertices, [x, y, 0.0]);

            var totWeightVal = 0.0;
            var totWeightSat = 0.0;
            var val = 0.0;
            
            for (var k = 0; k < dat.length; ++k) {
                var datPos = dat[k].slice(0, 2);
                var datVal = dat[k][2];
                var d = disp([x, y], datPos);
                // if (manhattan(d) < 0.25) {
                    var wVal = weight(d, spreadVal);
                    totWeightVal += wVal;
                    val += wVal * datVal;
                    totWeightSat += weight(d, spreadSat);
                // }
            }

            if (totWeightVal > 0.0) {
                val /= totWeightVal;
            }
            var sat = 1.0 - Math.exp(-totWeightSat * totWeightSat);

            verticesColor.push.apply(verticesColor, [val, val, val, sat]);
        }
    }

    verticesBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, verticesBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);

    verticesColorBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, verticesColorBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(verticesColor), gl.STATIC_DRAW);

    var vertexIndices = [];
    for (var i = 0; i < N - 1; ++i) {
        for (var j = 0; j < N - 1; ++j) {
            vertexIndices.push.apply(
                vertexIndices,
                [idx(i, j), idx(i + 1, j), idx(i + 1, j + 1), idx(i, j), idx(i, j + 1), idx(i + 1, j + 1)]);
        }
    }
    vertexIndicesBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, vertexIndicesBuffer);
    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(vertexIndices), gl.STATIC_DRAW);
}

function drawScene() {
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    gl.bindBuffer(gl.ARRAY_BUFFER, verticesBuffer);
    gl.vertexAttribPointer(vertexPositionAttribute, 3, gl.FLOAT, false, 0, 0);

    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, vertexIndicesBuffer);

    gl.bindBuffer(gl.ARRAY_BUFFER, verticesColorBuffer);
    gl.vertexAttribPointer(vertexColorAttribute, 4, gl.FLOAT, false, 0, 0);

    gl.drawElements(gl.TRIANGLES, nInd, gl.UNSIGNED_SHORT, 0);
}

function initShaders() {
    var fragmentShader = getShader(gl, "shader-fs");
    var vertexShader = getShader(gl, "shader-vs");

    shaderProgram = gl.createProgram();
    gl.attachShader(shaderProgram, vertexShader);
    gl.attachShader(shaderProgram, fragmentShader);
    gl.linkProgram(shaderProgram);

    if (!gl.getProgramParameter(shaderProgram, gl.LINK_STATUS)) {
        alert("Unable to initialize the shader program.");
    }

    gl.useProgram(shaderProgram);

    vertexPositionAttribute = gl.getAttribLocation(shaderProgram, "aVertexPosition");
    gl.enableVertexAttribArray(vertexPositionAttribute);

    vertexColorAttribute = gl.getAttribLocation(shaderProgram, "aVertexColor");
    gl.enableVertexAttribArray(vertexColorAttribute);
}

function getShader(gl, id) {
    var shaderScript = document.getElementById(id);

    if (!shaderScript) {
        return null;
    }

    var theSource = "";
    var currentChild = shaderScript.firstChild;

    while(currentChild) {
        if (currentChild.nodeType == 3) {
            theSource += currentChild.textContent;
        }

        currentChild = currentChild.nextSibling;
    }

    var shader;

    if (shaderScript.type == "x-shader/x-fragment") {
        shader = gl.createShader(gl.FRAGMENT_SHADER);
    } else if (shaderScript.type == "x-shader/x-vertex") {
        shader = gl.createShader(gl.VERTEX_SHADER);
    } else {
        return null;  // Unknown shader type
    }

    gl.shaderSource(shader, theSource);

    gl.compileShader(shader);

    if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
        alert("An error occurred compiling the shaders: " + gl.getShaderInfoLog(shader));
        return null;
    }

    return shader;
}
