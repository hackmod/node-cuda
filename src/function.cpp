#include <cstring>
#include <cstdio>
#include "function.hpp"
#include "types.h"

using namespace NodeCuda;

NAN_METHOD(LaunchKernel) {
  Nan::HandleScope scope;

  REQ_ARGS(4);
  NOCU_UNWRAP(function, NodeCUFunction, info[0]);

  Local<Array> gridDim = Local<Array>::Cast(info[1]);
  unsigned int gridDimX = Nan::To<uint32_t>(gridDim->Get(0)).ToChecked();
  unsigned int gridDimY = Nan::To<uint32_t>(gridDim->Get(1)).ToChecked();
  unsigned int gridDimZ = Nan::To<uint32_t>(gridDim->Get(2)).ToChecked();

  Local<Array> blockDim = Local<Array>::Cast(info[2]);
  unsigned int blockDimX = Nan::To<uint32_t>(blockDim->Get(0)).ToChecked();
  unsigned int blockDimY = Nan::To<uint32_t>(blockDim->Get(1)).ToChecked();
  unsigned int blockDimZ = Nan::To<uint32_t>(blockDim->Get(2)).ToChecked();

  NOCU_UNWRAP(devicePtr, NodeCUDeviceptr, info[3]);
  CUdeviceptr deviceptr = devicePtr->getRaw();

  Local<Object> buf = Nan::NewBuffer(sizeof(deviceptr)).ToLocalChecked();
  memcpy(Buffer::Data(buf), &deviceptr, sizeof(deviceptr));
  char *pbuffer = Buffer::Data(buf);
  size_t bufferSize = Buffer::Length(buf);

  void *cuExtra[] = {
    CU_LAUNCH_PARAM_BUFFER_POINTER, pbuffer,
    CU_LAUNCH_PARAM_BUFFER_SIZE,    &bufferSize,
    CU_LAUNCH_PARAM_END
  };

  CUresult error = cuLaunchKernel(function->getRaw(),
      gridDimX, gridDimY, gridDimZ,
      blockDimX, blockDimY, blockDimZ,
      0, 0, NULL, cuExtra);

  CHECK_ERR(error);

  info.GetReturnValue().Set(Nan::New<Integer>(error));
}

NAN_MODULE_INIT(NodeCuda::Function::Initialize) {
  Nan::SetMethod(target, "launchKernel", LaunchKernel);
}
