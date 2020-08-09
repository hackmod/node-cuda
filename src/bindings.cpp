#include "bindings.hpp"
#include "ctx.hpp"
#include "device.hpp"
#include "function.hpp"
#include "mem.hpp"
#include "module.hpp"

using namespace NodeCuda;

NAN_MODULE_INIT(init) {
  // Initiailze the cuda driver api
  cuInit(0);

  // These methods don't need instances
  Nan::SetAccessor(target, Nan::New("driverVersion").ToLocalChecked(), GetDriverVersion);
  Nan::SetAccessor(target, Nan::New("deviceCount").ToLocalChecked(), GetDeviceCount);

  // Initialize driver api bindings
  Ctx::Initialize(target);
  Device::Initialize(target);
  NodeCuda::Function::Initialize(target);
  Mem::Initialize(target);
  NodeCuda::Module::Initialize(target);
}

NAN_GETTER(NodeCuda::GetDriverVersion) {
  int driverVersion = 0;
  cuDriverGetVersion(&driverVersion);
  info.GetReturnValue().Set(Nan::New<Integer>(driverVersion));
}

NAN_GETTER(NodeCuda::GetDeviceCount) {
  int count = 0;
  cuDeviceGetCount(&count);
  info.GetReturnValue().Set(Nan::New<Integer>(count));
}

NODE_MODULE(cuda, init);
