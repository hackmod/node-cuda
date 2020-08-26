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
  Nan::SetMethod(target, "moduleUnload", ModuleUnload);
  Nan::SetMethod(target, "moduleGetFunction", ModuleGetFunction);
  Nan::SetMethod(target, "moduleGetGlobal", ModuleGetGlobal);
}

}  // namespace NodeCuda
