var n = 32;
var nTri = 2 * (n - 1) * (n - 1);
var nInd = 3 * nTri;

var canvas;
var gl;

var vertexBuffer;
var vertexIndicesBuffer;
var texCoordBuffer;
var alphaBuffer;

var vertexPositionAttribute;
var texCoordAttribute;
var alphaAttribute;

var tex;

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
        initTextures();
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
    var spreadVal = 0.15;
    var spreadAlpha = 0.2;

    function idx(i, j) {
        return i * n + j;
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
    var texCoord = [];
    var alpha = [];
    for (var i = 0; i < n; ++i) {
        var x = 2 * i / (n - 1) - 1;
        for (var j = 0; j < n; ++j) {
            var y = 2 * j / (n - 1) - 1;
            vertices.push.apply(vertices, [x, y, 0.0]);

            var totWeightVal = 0.0;
            var totWeightAlpha = 0.0;
            var val = 0.0;
            
            for (var k = 0; k < dat.length; ++k) {
                var datPos = dat[k].slice(0, 2);
                var datVal = dat[k][2];
                var d = disp([x, y], datPos);
                // if (manhattan(d) < 0.5) {
                    var wVal = weight(d, spreadVal);
                    totWeightVal += wVal;
                    val += wVal * datVal;
                    totWeightAlpha += weight(d, spreadAlpha);
                // }
            }

            if (totWeightVal > 0.0) {
                val /= totWeightVal;
            }
            var alphaVal = 1.0 - Math.exp(-totWeightAlpha * totWeightAlpha);

            texCoord.push(val);
            alpha.push(alphaVal);
        }
    }

    vertexBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vertexBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);

    texCoordBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, texCoordBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(texCoord), gl.STATIC_DRAW);
    
    alphaBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, alphaBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(alpha), gl.STATIC_DRAW);

    var vertexIndices = [];
    for (var i = 0; i < n - 1; ++i) {
        for (var j = 0; j < n - 1; ++j) {
            vertexIndices.push.apply(
                vertexIndices,
                [idx(i, j), idx(i + 1, j), idx(i + 1, j + 1), idx(i, j), idx(i, j + 1), idx(i + 1, j + 1)]);
        }
    }
    vertexIndicesBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, vertexIndicesBuffer);
    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(vertexIndices), gl.STATIC_DRAW);
    
    vertexIndicesBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, vertexIndicesBuffer);
    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(vertexIndices), gl.STATIC_DRAW);
}

function initTextures() {
    tex = gl.createTexture();
    var oneDTextureTexels = new Uint8Array([
        255,0,0,255, 
        0,255,0,255,
        0,0,255,255,
        255,0,0,255
    ]);
    var width = 4;
    var height = 1;
    gl.bindTexture(gl.TEXTURE_2D, tex);
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, width, height, 0, gl.RGBA, gl.UNSIGNED_BYTE, oneDTextureTexels);

    // gl.NEAREST is also allowed, instead of gl.LINEAR, as neither mipmap.
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);

    // Prevents s-coordinate wrapping (repeating).
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);

    // Prevents t-coordinate wrapping (repeating).
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
    gl.bindTexture(gl.TEXTURE_2D, null);
}

function drawScene() {
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    gl.bindBuffer(gl.ARRAY_BUFFER, vertexBuffer);
    gl.vertexAttribPointer(vertexPositionAttribute, 3, gl.FLOAT, false, 0, 0);
    
    gl.bindBuffer(gl.ARRAY_BUFFER, texCoordBuffer);
    gl.vertexAttribPointer(texCoordAttribute, 1, gl.FLOAT, false, 0, 0);
    
    gl.bindBuffer(gl.ARRAY_BUFFER, alphaBuffer);
    gl.vertexAttribPointer(alphaAttribute, 1, gl.FLOAT, false, 0, 0);

    gl.activeTexture(gl.TEXTURE0);
    gl.bindTexture(gl.TEXTURE_2D, tex);
    gl.uniform1i(gl.getUniformLocation(shaderProgram, "uTexture"), 0);

    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, vertexIndicesBuffer);
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

    vertexPositionAttribute = gl.getAttribLocation(shaderProgram, "aPosition");
    gl.enableVertexAttribArray(vertexPositionAttribute);
    
    texCoordAttribute = gl.getAttribLocation(shaderProgram, "aTexCoord");
    gl.enableVertexAttribArray(texCoordAttribute);
    
    alphaAttribute = gl.getAttribLocation(shaderProgram, "aAlpha");
    gl.enableVertexAttribArray(alphaAttribute);
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
