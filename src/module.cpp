#include "module.hpp"
#include "function.hpp"
#include "common.h"
#include "types.h"

namespace NodeCuda {

NAN_METHOD(ModuleLoad) {
  REQ_ARGS(1);

  Nan::Utf8String fname(info[0]);

  CUmodule module;
  CUresult error = cuModuleLoad(&module, *fname);

  CHECK_ERR(error);
  info.GetReturnValue().Set(NOCU_WRAP(NodeCUModule, module));
}

NAN_METHOD(ModuleUnload) {
  REQ_ARGS(1);

  NOCU_UNWRAP(module, NodeCUModule, info[0]);

  CUresult error = cuModuleUnload(module->getRaw());

  CHECK_ERR(error);
  info.GetReturnValue().Set(Nan::New<Integer>(error));
}

NAN_METHOD(ModuleLoadData) {
  REQ_ARGS(1);

  Nan::Utf8String image(info[0]);

  CUmodule module;
  CUresult error = cuModuleLoadData(&module, *image);

  if (error != CUDA_SUCCESS)
    info.GetReturnValue().Set(Nan::New<Integer>(error));
  else
    info.GetReturnValue().Set(NOCU_WRAP(NodeCUModule, module));
}

NAN_METHOD(ModuleLoadDataEx) {
  REQ_ARGS(1);

  Nan::Utf8String image(info[0]);
  int num = 0;
  CUjit_option* options = NULL;
  void** optionValues = NULL;
  if (info.Length() > 2 && info[1]->IsArray() && info[2]->IsArray()) {
    Local<Array> arr = Local<Array>::Cast(info[1]);
    Local<Array> arr2 = Local<Array>::Cast(info[2]);
    options = (CUjit_option*) malloc(sizeof(CUjit_option) * arr->Length());
    for (unsigned int j = 0; j < arr->Length(); j++) {
      options[j] = static_cast<CUjit_option>(Nan::To<uint32_t>(Nan::Get(arr, j).ToLocalChecked()).ToChecked());
      num++;
    }
    optionValues = (void **) malloc(sizeof(void*) * arr2->Length());
    for (unsigned int j = 0; j < arr2->Length(); j++) {
      Local<Value> target = Nan::Get(arr2, j).ToLocalChecked();
      if (Buffer::HasInstance(target)) {
        void* phost = Buffer::Data(target);
        optionValues[j] = phost;
      } else if (target->IsNumber()) {
        uint32_t val = Nan::To<uint32_t>(target).ToChecked();
        optionValues[j] = reinterpret_cast<void*>(val);
      }
    }
  }

  CUmodule module;
  CUresult error = cuModuleLoadDataEx(&module, *image, num, options, optionValues);

  if (options)
    free(options);
  if (optionValues)
    free(optionValues);

  if (error != CUDA_SUCCESS)
    info.GetReturnValue().Set(Nan::New<Integer>(error));
  else
    info.GetReturnValue().Set(NOCU_WRAP(NodeCUModule, module));
}

NAN_METHOD(ModuleGetFunction) {
  REQ_ARGS(2);

  NOCU_UNWRAP(module, NodeCUModule, info[0]);

  Nan::Utf8String name(info[1]);

  CUfunction function;
  CUresult error = cuModuleGetFunction(&function, module->getRaw(), *name);

  CHECK_ERR(error);
  info.GetReturnValue().Set(NOCU_WRAP(NodeCUFunction, function));
}

NAN_METHOD(ModuleGetGlobal) {
  REQ_ARGS(2);

  NOCU_UNWRAP(module, NodeCUModule, info[0]);

  Nan::Utf8String name(info[1]);

  CUdeviceptr dptr;
  size_t bytes;
  CUresult error = cuModuleGetGlobal(&dptr, &bytes, module->getRaw(), *name);
  CHECK_ERR(error);

  if (info.Length() >= 3 && !info[2]->IsUndefined()) {
    Local<Object> obj = Nan::To<Object>(info[2]).ToLocalChecked();
    Nan::Set(obj, 0, Nan::New<Integer>(static_cast<int32_t>(bytes))); // FIXME
  }
  info.GetReturnValue().Set(NOCU_WRAP(NodeCUDeviceptr, dptr));
}

NAN_MODULE_INIT(Module::Initialize) {
  // Module objects can only be created by load functions
  Nan::SetMethod(target, "moduleLoad", ModuleLoad);
  Nan::SetMethod(target, "moduleLoadData", ModuleLoadData);
  Nan::SetMethod(target, "moduleLoadDataEx", ModuleLoadDataEx);
  Nan::SetMethod(target, "moduleUnload", ModuleUnload);
  Nan::SetMethod(target, "moduleGetFunction", ModuleGetFunction);
  Nan::SetMethod(target, "moduleGetGlobal", ModuleGetGlobal);
}

}  // namespace NodeCuda
