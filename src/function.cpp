#include <cstring>
#include <cstdio>
#include "function.hpp"
#include "mem.hpp"

using namespace NodeCuda;

Nan::Persistent<FunctionTemplate> NodeCuda::Function::constructor;

NAN_MODULE_INIT(NodeCuda::Function::Initialize) {
  Local<FunctionTemplate> t = Nan::New<FunctionTemplate>(NodeCuda::Function::New);
  t->InstanceTemplate()->SetInternalFieldCount(1);
  t->SetClassName(Nan::New("CudaFunction").ToLocalChecked());

  Nan::SetPrototypeMethod(t, "launchKernel", NodeCuda::Function::LaunchKernel);

  target->Set(Nan::New("Function").ToLocalChecked(), t->GetFunction());
  NodeCuda::Function::constructor.Reset(t);
  // Function objects can only be created by cuModuleGetFunction
}

NAN_METHOD(NodeCuda::Function::New) {
  NodeCuda::Function *pfunction = new NodeCuda::Function();
  pfunction->Wrap(info.Holder());

  info.GetReturnValue().Set(info.Holder());
}

NAN_METHOD(NodeCuda::Function::LaunchKernel) {
  Function *pfunction = ObjectWrap::Unwrap<Function>(info.Holder());

  Local<Array> gridDim = Local<Array>::Cast(info[0]);
  unsigned int gridDimX = Nan::To<uint32_t>(gridDim->Get(0)).ToChecked();
  unsigned int gridDimY = Nan::To<uint32_t>(gridDim->Get(1)).ToChecked();
  unsigned int gridDimZ = Nan::To<uint32_t>(gridDim->Get(2)).ToChecked();

  Local<Array> blockDim = Local<Array>::Cast(info[1]);
  unsigned int blockDimX = Nan::To<uint32_t>(blockDim->Get(0)).ToChecked();
  unsigned int blockDimY = Nan::To<uint32_t>(blockDim->Get(1)).ToChecked();
  unsigned int blockDimZ = Nan::To<uint32_t>(blockDim->Get(2)).ToChecked();

  Local<Object> buf = Nan::To<Object>(info[2]).ToLocalChecked();
  char *pbuffer = Buffer::Data(buf);
  size_t bufferSize = Buffer::Length(buf);

  void *cuExtra[] = {
    CU_LAUNCH_PARAM_BUFFER_POINTER, pbuffer,
    CU_LAUNCH_PARAM_BUFFER_SIZE,    &bufferSize,
    CU_LAUNCH_PARAM_END
  };

  CUresult error = cuLaunchKernel(pfunction->m_function,
      gridDimX, gridDimY, gridDimZ,
      blockDimX, blockDimY, blockDimZ,
      0, 0, NULL, cuExtra);

  info.GetReturnValue().Set(Nan::New<Integer>(error));
}

