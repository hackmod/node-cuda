#include <cstring>
#include <cstdio>
#include "function.hpp"
#include "types.h"

using namespace NodeCuda;

NAN_METHOD(LaunchKernel) {
  Nan::HandleScope scope;

  REQ_ARGS(3);
  NOCU_UNWRAP(function, NodeCUFunction, info[0]);

  Local<Array> gridDim = Local<Array>::Cast(info[1]);
  unsigned int gridDimX = Nan::To<uint32_t>(gridDim->Get(0)).ToChecked();
  unsigned int gridDimY = Nan::To<uint32_t>(gridDim->Get(1)).ToChecked();
  unsigned int gridDimZ = Nan::To<uint32_t>(gridDim->Get(2)).ToChecked();

  Local<Array> blockDim = Local<Array>::Cast(info[2]);
  unsigned int blockDimX = Nan::To<uint32_t>(blockDim->Get(0)).ToChecked();
  unsigned int blockDimY = Nan::To<uint32_t>(blockDim->Get(1)).ToChecked();
  unsigned int blockDimZ = Nan::To<uint32_t>(blockDim->Get(2)).ToChecked();

  void *cuExtra[] = {
    CU_LAUNCH_PARAM_BUFFER_POINTER, 0,
    CU_LAUNCH_PARAM_BUFFER_SIZE,    0,
    CU_LAUNCH_PARAM_END
  };
  void **extra = NULL;
  char *pbuffer;
  size_t bufferSize;

  if (info.Length() > 3 && !info[3]->IsUndefined() && !info[3]->IsNull()) {
    Local<Object> buf = Nan::To<Object>(info[3]).ToLocalChecked();
    pbuffer = Buffer::Data(buf);
    bufferSize = Buffer::Length(buf);

    cuExtra[1] = pbuffer;
    cuExtra[3] = &bufferSize;
    extra = cuExtra;
  }

  CUstream custream = NULL;

  if (info.Length() > 4 && !info[4]->IsUndefined() && !info[4]->IsNull()) {
    NOCU_UNWRAP(stream, NodeCUStream, info[4]);
    custream = stream->getRaw();
  }

  CUresult error = cuLaunchKernel(function->getRaw(),
      gridDimX, gridDimY, gridDimZ,
      blockDimX, blockDimY, blockDimZ,
      0, custream ? custream : 0, NULL, extra);

  CHECK_ERR(error);

  info.GetReturnValue().Set(Nan::New<Integer>(error));
}

NAN_MODULE_INIT(NodeCuda::Function::Initialize) {
  Nan::SetMethod(target, "launchKernel", LaunchKernel);
}
