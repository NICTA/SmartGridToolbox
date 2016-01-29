var Dexter = Dexter || {};

Dexter.Heatmap = (function() {
    console.log("Loaded Dexter.Heatmap.");

    // The grid looks like this:
    //
    // +   +   +   +
    //   .   .   .  
    // +   +   +   +
    //   .   .   .  
    // +   +   +   +
    //   .   .   .  
    // +   +   +   +
    //
    // + = vertices at edges of grid squares (0 .. n^2 - 1)
    // . = vertices at center of grid squares (n^2 .. n^2 + nm1^2 - 1)
    //
    // Triangles are formed by joining the center of each square to the corners.
    
    var n = 32; // n x n grid.
    var n2 = n * n; // Number of grid points.
    var nm1 = n - 1; // Number of grid intervals in each dimension.
    var nm12 = nm1 * nm1; // Number of grid squares.

    var nVert = n2 + nm12; // A vertex for each grid point, plus one at the center of each square.
    var nTri = 4 * nm12; // Each grid square has four triangles.
    var nTriVert = 3 * nTri; // Each triangle has three vertices. 

    var xMin = -1;
    var xMax = 1;
    var yMin = -1;
    var yMax = 1;

    var nCols = 256; // Number of colors in colormap.
    
    var spreadVal = 0.05; // Characteristic radius around each data point for calculating value on grid.
    var spreadAlpha = 0.03; // Characteristic radius around each data point for calculating alpha on grid.
    
    var heatMap = {
        n: 6,
        x: [0.00, 0.20, 0.40, 0.60, 0.80, 1.00],
        r: [0,   0,   0,   128, 255, 255],
        g: [0,   128, 255, 255, 255, 128],
        b: [255, 255, 255, 128, 0,   0  ],
        a: [255, 255, 255, 255, 255, 255]
    }; // Determines the colormap.

    var canvas;
    var gl;

    var vertices;
    var vertexBuffer;

    var triVertices;
    var triVertexBuffer;

    var texCoordBuffer;
    var alphaBuffer;

    var vertexPositionAttribute;
    var texCoordAttribute;
    var alphaAttribute;

    var tex;

    var shaderProgram;

    var dat;

    var vertexShaderSrc = "\
precision lowp float;\
attribute vec3 aPosition;\
attribute float aTexCoord;\
attribute float aAlpha;\
varying float vTexCoord;\
varying float vAlpha;\
\
void main(void) {\
vTexCoord = aTexCoord;\
vAlpha = aAlpha;\
gl_Position = vec4(aPosition, 1.0);\
}\
    ";

    var fragmentShaderSrc = "\
precision lowp float;\
uniform sampler2D uTexture;\
varying float vTexCoord;\
varying float vAlpha;\
\
void main(void) {\
vec4 color = texture2D(uTexture, vec2(vTexCoord, 0.5));\
color.a = vAlpha;\
gl_FragColor = color;\
}\
    ";

    function init(canv) {
        canvas = canv;

        initWebGL(canvas);      // Initialize the GL context

        // Only continue if WebGL is available and working

        if (gl) {
            gl.enable(gl.BLEND);
            gl.blendEquation(gl.FUNC_ADD);
            gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);
            gl.clearColor(1, 1, 1, 1);  // Clear to black, fully opaque
            gl.clearDepth(1.0);                 // Clear everything

            initShaders();
            initBuffers();
            initTextures();
            setData(testData());
        }
    }
    
    function setViewRect(xMin_, yMin_, xMax_, yMax_) {
        xMin = xMin_;
        yMin = yMin_;
        xMax = xMax_;
        yMax = yMax_;
    }
    
    function setViewRectToCanvas() {
        xMin = 0;
        yMin = 0;
        xMax = canvas.width;
        yMax = canvas.height;
    }
    
    function setData(dat) {
        this.dat = dat;

        var texCoord = [];
        var alpha = [];
        for (var i = 0; i < nVert; ++i) {
            var xy = vertexXy(i);

            var totWeightVal = 0.0;
            var totWeightAlpha = 0.0;
            var val = 0.0;

            for (var k = 0; k < dat.length; ++k) {
                var datPos = dat[k].slice(0, 2);
                var datVal = dat[k][2];
                var d = disp(xy, datPos);
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

        texCoordBuffer = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, texCoordBuffer);
        gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(texCoord), gl.STATIC_DRAW);

        alphaBuffer = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, alphaBuffer);
        gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(alpha), gl.STATIC_DRAW);

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
    };

    function testData(nMin, nMax) {
        function rand(min, max)
        {
            return min + Math.random() * (max - min);
        }

        var nDat = Math.floor(rand(nMin, nMax));
        var dat = [];
        for (var i = 0; i < nDat; ++i)
        {
            dat.push([rand(-1, 1), rand(-1, 1), rand(0, 1)]);
        }
        return dat;
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

    function initShaders() {
        var vertexShader = getShader(gl, vertexShaderSrc, gl.VERTEX_SHADER);
        var fragmentShader = getShader(gl, fragmentShaderSrc, gl.FRAGMENT_SHADER);

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

    function initBuffers() {
        initVertices();
        vertexBuffer = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, vertexBuffer);
        gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);

        initTriangles();
        triVertexBuffer = gl.createBuffer();
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, triVertexBuffer);
        gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(triVertices), gl.STATIC_DRAW);
    };

    function initTextures() {
        tex = gl.createTexture();
        var cols = [];
        for (var i = 0; i < nCols; ++i) {
            var x = (i / (nCols - 1));
            cols.push(colorAt(x));
        }
        cols = [].concat.apply([], cols); // Flatten.
        var oneDTextureTexels = new Uint8Array(cols);
        gl.bindTexture(gl.TEXTURE_2D, tex);
        gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, nCols, 1, 0, gl.RGBA, gl.UNSIGNED_BYTE, oneDTextureTexels);

        // gl.NEAREST is also allowed, instead of gl.LINEAR, as neither mipmap.
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);

        // Prevents s-coordinate wrapping (repeating).
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);

        // Prevents t-coordinate wrapping (repeating).
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
        gl.bindTexture(gl.TEXTURE_2D, null);
    }

    function draw() {
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

        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, triVertexBuffer);
        gl.drawElements(gl.TRIANGLES, nTriVert, gl.UNSIGNED_SHORT, 0);
    }

    function getShader(gl, src, shaderType) {
        var shader = gl.createShader(shaderType);
        gl.shaderSource(shader, src);
        gl.compileShader(shader);
        if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
            alert("An error occurred compiling the shaders: " + gl.getShaderInfoLog(shader));
            return null;
        }
        return shader;
    }

    function initVertices() {
        function gridCoord(i) {
            return 2 * i / nm1 - 1;
        }

        function centerCoord(i) {
            return gridCoord(i + 0.5);
        }

        vertices = [];
        for (var i = 0; i < n; ++i) {
            var x = gridCoord(i);
            for (var j = 0; j < n; ++j) {
                var y = gridCoord(j);
                vertices.push.apply(vertices, [x, y, 0.0]);
            }
        }
        for (var i = 0; i < n - 1; ++i) {
            var x = centerCoord(i);
            for (var j = 0; j < n - 1; ++j) {
                var y = centerCoord(j);
                vertices.push.apply(vertices, [x, y, 0.0]);
            }
        }
    }

    function initTriangles() {
        function cornerIdx(i, j) {
            return i * n + j;
        }
        
        function centerIdx(i, j) {
            return n2 + i * nm1 + j;
        }

        triVertices = [];
        for (var i = 0; i < n - 1; ++i) {
            for (var j = 0; j < n - 1; ++j) {
                triVertices.push.apply(
                    triVertices,
                    [
                        centerIdx(i, j), cornerIdx(i, j), cornerIdx(i, j + 1),
                        centerIdx(i, j), cornerIdx(i, j + 1), cornerIdx(i + 1, j + 1),
                        centerIdx(i, j), cornerIdx(i + 1, j + 1), cornerIdx(i + 1, j),
                        centerIdx(i, j), cornerIdx(i + 1, j), cornerIdx(i, j)
                    ]
                );
            }
        }
    }
        
    function vertexXy(i) {
        return vertices.slice(3 * i, 3 * i + 2);
    }

    function colorAt(x) { 
        var result;

        if (x <= 0) {
            result = [
                heatMap.r[0],
                heatMap.g[0],
                heatMap.b[0],
                heatMap.a[0]
            ];
        } else if (x >= 1) {
            result = [
                heatMap.r[heatMap.n - 1],
                heatMap.g[heatMap.n - 1],
                heatMap.b[heatMap.n - 1],
                heatMap.a[heatMap.n - 1]
            ];
        } else {
            var i1 = 0;
            for (i1 = 1; i1 < heatMap.n; ++i1)
            {
                if (heatMap.x[i1] > x) break;
            }
            var i0 = i1 - 1;
            var x0 = heatMap.x[i0];
            var x1 = heatMap.x[i1];
            var fx0 = (x1 - x) / (x1 - x0);
            var fx1 = (x - x0) / (x1 - x0);

            result = [
                fx0 * heatMap.r[i0] + fx1 * heatMap.r[i1],
                fx0 * heatMap.g[i0] + fx1 * heatMap.g[i1],
                fx0 * heatMap.b[i0] + fx1 * heatMap.b[i1],
                fx0 * heatMap.a[i0] + fx1 * heatMap.a[i1]
            ];
        }
        return result;
    };

    return {
        init: init,
        setData: setData,
        setViewRect: setViewRect,
        setViewRectToCanvas: setViewRectToCanvas,
        draw: draw,
        testData: testData
    };
}());

function testDexterHeatmap()
{
    var canvas = document.getElementById("glcanvas");
    Dexter.Heatmap.init(canvas);
    Dexter.Heatmap.setViewRectToCanvas();
    setInterval(function() {Dexter.Heatmap.setData(Dexter.Heatmap.testData(200, 200)); Dexter.Heatmap.draw();}, 10);
    // Dexter.Heatmap.setData(Dexter.Heatmap.testData(1, 10)); Dexter.Heatmap.draw();
}
