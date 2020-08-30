var Buffer = require('buffer').Buffer;
var cu = require('../cuda');

//cuDriverGetVersion
//cuDeviceGetCount
console.log("Node-cuda exports:", cu);

for (var i = 0; i < cu.deviceCount; i++) {
    //cuDeviceGet
    var cuDevice = cu.deviceGet(i);

    //cuDeviceComputeCapability
    //cuDeviceGetName
    //cuDeviceTotalMem
    console.log("Device " + i + ":", cuDevice);
}

//cuCtxCreate
var cuCtx = cu.ctxCreate(0, cu.deviceGet(0));

//cuCtxGetApiVersion
console.log("Created context:", cuCtx);

//cuMemAllocPitch
var cuMem = cu.memAllocPitch(100, 100, 8);
console.log("Allocated 100x100 array of doubles:", cuMem);

//cuMemFree
var error = cu.memFree(cuMem);
console.log("Mem Free with error code: " + error);

//cuMemAlloc
var cuMem = cu.memAlloc(100);
console.log("Allocated 100 bytes:", cuMem);

var buf = new Buffer(100);
for (var i = 0; i < buf.length; i++) {
    buf[i] = (i + 1) % 256;
}
console.log("Created buffer of 100 bytes:", buf);

// cuMemcpyHtoD
var error = cu.memcpyHtoD(cuMem, buf);
console.log("Copied buffer to device:", error);

//cuModuleLoad
var cuModule = cu.moduleLoad("test/test.fatbin");
console.log("Loaded module:", cuModule);

//cuModuleGetFunction
var cuFunction = cu.moduleGetFunction(cuModule, "helloWorld");
console.log("Got function:", cuFunction);

var len = new Uint32Array(1);
var cuGlobal = cu.moduleGetGlobal(cuModule, "d_size", len);
console.log("Got global:", cuGlobal, len);

//cuLaunchKernel
var error = cu.launch(cuFunction, [3, 1, 1], [2, 2, 2],
[{
    type: "DevicePtr",
    value: cuMem.getRaw()
}]);
console.log("Launched kernel:", error);

// cuMemcpyDtoH
var error = cu.memcpyDtoH(buf, cuMem, true);
console.log("Copied buffer to host:", error);

var error = cu.ctxSynchronize(cuCtx);

console.log("Context destroyed with error code: " + error);
/*
//cuCtxSynchronize
var error = cuCtx.synchronize(function(error) {
    console.log("Context synchronize with error code: " + error);

    //cuMemFree
    var error = cuMem.free();
    console.log("Mem Free with error code: " + error);

    //cuCtxDestroy
    error = cuCtx.destroy();
    console.log("Context destroyed with error code: " + error);
});

*/
