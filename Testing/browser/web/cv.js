var vid;
var can1;
var can2;
var wid;
var hei;
var out;
var lastwasm = 0;
var jst;
var wasmt;
var NBPARAMS = 2;

var init_func = uiconfig["sched_init"];
var run_func = uiconfig["sched_run"];


(function() {
    var dst1 = null;
    var dst2 = null;
    var src = null;
    var params1 = null;
    var params2 = null;
    var obj1 = null;
    var obj2 = null;
    var obj1_id = null;
    var obj2_id = null;
    window.addEventListener('DOMContentLoaded', function() {
        var isStreaming = false;
        vid = document.getElementById('vid');
        can1 = document.getElementById('mycanvas1');
        can2 = document.getElementById('mycanvas2');
        out = document.getElementById('updatetime');
        if (can1)
        {
           con1 = can1.getContext('2d',{willReadFrequently:true});
        }
        if (can2)
        {
           con2 = can2.getContext('2d',{willReadFrequently:true});
        }
        wid = uiconfig["width"];
        hei = uiconfig["height"];
        var constraints = {
            audio: false,
            video: {
                width: wid,
                height: hei
            }
        };

        //console.log(uiconfig);
        if ("param1" in uiconfig)
        {
            algo1 = document.getElementById('algo1');
            algo1.textContent=uiconfig["param1"]["name"];

            var gui1 = new dat.GUI({closeOnTop:false,autoPlace:false,name: 'Algo1'});
            let guiContainer1 = document.getElementById('guiContainer1');
            guiContainer1.appendChild(gui1.domElement);
    
            obj1={};
            obj1_id = [];
            var cfg = uiconfig["param1"]["values"];
            var arrayLength = cfg.length;
            for (var i = 0; i < arrayLength; i++) {
               var s = cfg[i];
               var v = s["value"];
               var n = s["name"];
               obj1[n]=v;
               obj1_id.push(n);
    
               gui1.add(obj1, n).min(s["min"]).max(s["max"]).step(s["step"]);
    
            }

            gui1.open();

        }
        
        if ("param2" in uiconfig)
        {
            algo2 = document.getElementById('algo2');
            algo2.textContent=uiconfig["param2"]["name"];
    
            
    
            var gui2 = new dat.GUI({closeOnTop:false,autoPlace:false,name: 'Algo2'});
            let guiContainer2 = document.getElementById('guiContainer2');
            guiContainer2.appendChild(gui2.domElement);
    
            obj2={};
            obj2_id = [];
            cfg = uiconfig["param2"]["values"];
            arrayLength = cfg.length;
            for (var i = 0; i < arrayLength; i++) {
               var s = cfg[i];
               var v = s["value"];
               var n = s["name"];
               obj2[n]=v;
               obj2_id.push(n);
    
               gui2.add(obj2, n).min(s["min"]).max(s["max"]).step(s["step"]);
    
            }

            gui2.open();

        }
      


  


//1.        navigator.mediaDevices.getUserMedia(constraints)
      navigator.mediaDevices.getUserMedia(constraints)
            .then(function(mediaStream) {
                var vid = document.querySelector('video');
                vid.srcObject = mediaStream;
                vid.onloadedmetadata = function(e) {
                    vid.play();
                };
            })
            .catch(function(err) {
                console.log(err.message);
            });             
        // Wait until the video stream can play
       vid.addEventListener('canplay', function(e) {
            if (!isStreaming) {
                // videoWidth isn't always set correctly in all browsers
                if (vid.videoWidth > 0) hei = vid.videoHeight / (vid.videoWidth / wid);
                
                if (can1)
                {
                   can1.setAttribute('width', wid);
                   can1.setAttribute('height', hei);
                   // Reverse the canvas image
                   con1.translate(wid, 0);
                   con1.scale(-1, 1);
                }

                if (can2)
                {
                   can2.setAttribute('width', wid);
                   can2.setAttribute('height', hei);
                   // Reverse the canvas image
                   con2.translate(wid, 0);
                   con2.scale(-1, 1);
                }

                isStreaming = true;
            }
        }, false);  

        // Wait until the video stream can play
//2.        vid.addEventListener('canplay', function(e) {
           

        // Wait for the video to start to play
    vid.addEventListener('play', function() {
            //Setup image memory
            var id = con1.getImageData(0, 0, can1.width, can1.height);
            var d = id.data;
            dst1 = _malloc(d.length);
            dst2 = _malloc(d.length);
            src = _malloc(d.length);
            if ("param1" in uiconfig)
            {
               params1 = _malloc(obj1_id.length*4);
            }
            if ("param2" in uiconfig)
            {
               params2 = _malloc(obj2_id.length*4);
            }
            //console.log("What " + d.length);
            var result = Module.ccall(init_func, // name of C function
                                    'number', // return type
                                    ['number','number','number','number','number'], // argument types
                                    [src,dst1,dst2,params1,params2]); // arguments  
            var errorOccured = 0;
            if (result!=0)
            {
                console.log("Error initializing CV scheduler");
                errorOccured = 1;
            }
           

            wasmt = new TimeSeries();

            var smoothie = new SmoothieChart({
                grid: {
                    strokeStyle: 'rgb(125, 0, 0)',
                    fillStyle: 'rgb(60, 0, 0)',
                    lineWidth: 1,
                    millisPerLine: 250,
                    verticalSections: 6
                }
            });
            smoothie.addTimeSeries(wasmt, {
                strokeStyle: 'rgb(255, 0, 255)',
                fillStyle: 'rgba(255, 0, 255, 0.3)',
                lineWidth: 3
            });

            smoothie.streamTo(document.getElementById("chart"), 1000);



            if (errorOccured==0)
            {
               sFilter();
            }
            

        });

    });

    function updateChart(wasmtime) {

        if (wasmtime > 0) {
            wasmt.append(new Date().getTime(), wasmtime);
            lastwasm = wasmtime;
        } else {
            wasmt.append(new Date().getTime(), lastwasm);
        }


    }



    //Request Animation Frame function
    var sFilter = function() {
        if (vid.paused || vid.ended) return;
        var t0 = performance.now();
        var t1;
        //console.log(wid + " " + hei);
        con1.fillRect(0, 0, wid, hei);
        con1.drawImage(vid, 0, 0, wid, hei);
        var imageData = con1.getImageData(0, 0, can1.width, can1.height);
        var data = imageData.data;
        
            HEAPU8.set(data, src);

            if ("param1" in uiconfig)
            {
                for (var i = 0; i < obj1_id.length; i++) 
                {
                    HEAPU32[(params1>>2)+i]=obj1[obj1_id[i]];
                }
            }

            if ("param2" in uiconfig)
            {
                for (var i = 0; i < obj2_id.length; i++) 
                {
                    HEAPU32[(params2>>2)+i]=obj2[obj2_id[i]];
                }
            }


            var result = Module.ccall(run_func, // name of C function
                'number', // return type
                ['number', 'number','number','number','number'], // argument types
                [src,dst1, dst2,params1,params2]); // arguments				
			t1 = performance.now();
            if ((result == 0) || (result == -9))
            {
               if (can1)
               {
                 var result = HEAPU8.subarray(dst1, dst1 + data.length);
                 imageData.data.set(result);
                 con1.putImageData(imageData, 0, 0);
               }

               if (can2)
               {
                  var result = HEAPU8.subarray(dst2, dst2 + data.length);
                  imageData.data.set(result);
                  con2.putImageData(imageData, 0, 0);
               }
        

               out.value = (t1 - t0).toFixed(2);
               updateChart((t1 - t0).toFixed(2));
        
               window.requestAnimationFrame(sFilter);
           }
           else 
           {
             console.log("Stream scheduler error = " + result);
           }
    }
})();
