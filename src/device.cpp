#include "device.hpp"
#include "types.h"

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
  CUresult error = cuDeviceGet(&device, ordinal);
  CHECK_ERR(error);

  info.GetReturnValue().Set(Nan::New<Integer>(static_cast<uint32_t>(device)));
}

NAN_METHOD(ComputeCapability) {
  CUdevice device = Nan::To<uint32_t>(info[0]).ToChecked();

  int major = 0, minor = 0;
  CUresult error = cuDeviceComputeCapability(&major, &minor, device);
  CHECK_ERR(error);

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
  CUresult error = cuDeviceTotalMem(&totalGlobalMem, device);
  CHECK_ERR(error);

  double value = totalGlobalMem;
  info.GetReturnValue().Set(Nan::New(value));
}

NAN_METHOD(DevicePrimaryCtxReset) {
  CUdevice device = Nan::To<uint32_t>(info[0]).ToChecked();
  CUresult error = cuDevicePrimaryCtxReset(device);
  CHECK_ERR(error);

  info.GetReturnValue().Set(Nan::New<Integer>(error));
}

NAN_MODULE_INIT(Device::Initialize) {
  Nan::SetMethod(target, "deviceGet", DeviceGet);
  Nan::SetMethod(target, "deviceComputeCapability", ComputeCapability);
  Nan::SetMethod(target, "deviceGetName", GetName);
  Nan::SetMethod(target, "deviceTotalMem", TotalMem);
  Nan::SetMethod(target, "devicePrimaryCtxReset", DevicePrimaryCtxReset);
}

}  // namespace NodeCuda
