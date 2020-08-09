#include "device.hpp"

using namespace NodeCuda;

Nan::Persistent<FunctionTemplate> Device::constructor;

NAN_MODULE_INIT(Device::Initialize) {
  Local<FunctionTemplate> t = Nan::New<FunctionTemplate>(Device::New);
  t->InstanceTemplate()->SetInternalFieldCount(1);
  t->SetClassName(Nan::New("CudaDevice").ToLocalChecked());

  Nan::SetAccessor(t->InstanceTemplate(), Nan::New("name").ToLocalChecked(), Device::GetName);
  Nan::SetAccessor(t->InstanceTemplate(), Nan::New("totalMem").ToLocalChecked(), Device::GetTotalMem);
  Nan::SetAccessor(t->InstanceTemplate(), Nan::New("computeCapability").ToLocalChecked(), Device::GetComputeCapability);

  target->Set(Nan::New("Device").ToLocalChecked(), t->GetFunction());
  Device::constructor.Reset(t);
}

static Handle<Value> GetName_(CUdevice device) {
  char deviceName[256];

  cuDeviceGetName(deviceName, 256, device);
  Local<String> result = Nan::New(deviceName).ToLocalChecked();
  return result;
}

NAN_METHOD(Device::New) {
  Local<Object> result = info.Holder();
  int ordinal = Nan::To<int32_t>(info[0]).ToChecked();

  if (!Nan::New(Device::constructor)->HasInstance(result)) {
    result = Nan::New<FunctionTemplate>(Device::constructor)->InstanceTemplate()->NewInstance();
  }

  Device *pdevice = new Device();
  cuDeviceGet(&(pdevice->m_device), ordinal);

  pdevice->Wrap(result);
  info.GetReturnValue().Set(result);
}

NAN_GETTER(Device::GetComputeCapability) {
  Device *pdevice = ObjectWrap::Unwrap<Device>(info.Holder());
  int major = 0, minor = 0;
  cuDeviceComputeCapability(&major, &minor, pdevice->m_device);

  Local<Object> result = Nan::New<Object>();
  result->Set(Nan::New("major").ToLocalChecked(), Nan::New<Integer>(major));
  result->Set(Nan::New("minor").ToLocalChecked(), Nan::New<Integer>(minor));
  info.GetReturnValue().Set(result);
}

NAN_GETTER(Device::GetName) {
  Device *pdevice = ObjectWrap::Unwrap<Device>(info.Holder());
  info.GetReturnValue().Set(GetName_(pdevice->m_device));
}

NAN_GETTER(Device::GetTotalMem) {
  Device *pdevice = ObjectWrap::Unwrap<Device>(info.Holder());
  size_t totalGlobalMem;
  cuDeviceTotalMem(&totalGlobalMem, pdevice->m_device);

  info.GetReturnValue().Set(Nan::New<Integer>(static_cast<uint32_t>(totalGlobalMem)));
}
