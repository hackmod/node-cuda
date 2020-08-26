#include "device.hpp"

namespace NodeCuda {

static Handle<Value> GetName_(CUdevice device) {
  char deviceName[256];

  cuDeviceGetName(deviceName, 256, device);
  Local<String> result = Nan::New(deviceName).ToLocalChecked();
  return result;
}

NAN_METHOD(DeviceGet) {
  int ordinal = Nan::To<int32_t>(info[0]).ToChecked();

  CUdevice device;
  cuDeviceGet(&device, ordinal);

  info.GetReturnValue().Set(Nan::New<Integer>(static_cast<uint32_t>(device)));
}

NAN_METHOD(ComputeCapability) {
  CUdevice device = Nan::To<uint32_t>(info[0]).ToChecked();

  int major = 0, minor = 0;
  cuDeviceComputeCapability(&major, &minor, device);

  Local<Object> result = Nan::New<Object>();
  result->Set(Nan::New("major").ToLocalChecked(), Nan::New<Integer>(major));
  result->Set(Nan::New("minor").ToLocalChecked(), Nan::New<Integer>(minor));
  info.GetReturnValue().Set(result);
}

NAN_METHOD(GetName) {
  CUdevice device = Nan::To<uint32_t>(info[0]).ToChecked();
  info.GetReturnValue().Set(GetName_(device));
}

NAN_METHOD(TotalMem) {
  CUdevice device = Nan::To<uint32_t>(info[0]).ToChecked();
  size_t totalGlobalMem;
  cuDeviceTotalMem(&totalGlobalMem, device);

  info.GetReturnValue().Set(Nan::New<Integer>(static_cast<uint32_t>(totalGlobalMem)));
}

NAN_MODULE_INIT(Device::Initialize) {
  Nan::SetMethod(target, "deviceGet", DeviceGet);
  Nan::SetMethod(target, "deviceComputeCapability", ComputeCapability);
  Nan::SetMethod(target, "deviceGetName", GetName);
  Nan::SetMethod(target, "deviceTotalMem", TotalMem);
}

}  // namespace NodeCuda
