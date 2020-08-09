#include <cstring>
#include <stdio.h>
#include "mem.hpp"

using namespace NodeCuda;

Nan::Persistent<FunctionTemplate> Mem::constructor;

NAN_MODULE_INIT(Mem::Initialize) {
  Local<FunctionTemplate> t = Nan::New<FunctionTemplate>(Mem::New);
  t->InstanceTemplate()->SetInternalFieldCount(1);
  t->SetClassName(Nan::New("CudaMem").ToLocalChecked());

  // Mem objects can only be created by allocation functions
  Nan::SetMethod(target, "memAlloc", Mem::Alloc);
  Nan::SetMethod(target, "memAllocPitch", Mem::AllocPitch);

  Nan::SetAccessor(t->InstanceTemplate(), Nan::New("devicePtr").ToLocalChecked(), Mem::GetDevicePtr);

  Nan::SetPrototypeMethod(t, "free", Mem::Free);
  Nan::SetPrototypeMethod(t, "copyHtoD", Mem::CopyHtoD);
  Nan::SetPrototypeMethod(t, "copyDtoH", Mem::CopyDtoH);

  target->Set(Nan::New("Mem").ToLocalChecked(), t->GetFunction());
  Mem::constructor.Reset(t);
}

NAN_METHOD(Mem::New) {
  if (info.IsConstructCall()) {
    Mem *pmem = new Mem();
    pmem->Wrap(info.This());

    info.GetReturnValue().Set(info.This());
  }
}

NAN_METHOD(Mem::Alloc) {
  Local<Object> result = Nan::New<FunctionTemplate>(Mem::constructor)->InstanceTemplate()->NewInstance();
  Mem *pmem = new Mem();
  pmem->Wrap(result);

  size_t bytesize = Nan::To<uint32_t>(info[0]).ToChecked();
  CUresult error = cuMemAlloc(&(pmem->m_devicePtr), bytesize);

  result->Set(Nan::New("size").ToLocalChecked(), Nan::New<Integer>(static_cast<uint32_t>(bytesize)));
  result->Set(Nan::New("error").ToLocalChecked(), Nan::New<Integer>(error));

  info.GetReturnValue().Set(result);
}

NAN_METHOD(Mem::AllocPitch) {
  Local<Object> result = Nan::New<FunctionTemplate>(Mem::constructor)->InstanceTemplate()->NewInstance();
  Mem *pmem = new Mem();
  pmem->Wrap(result);

  size_t pPitch;
  unsigned int ElementSizeBytes = Nan::To<uint32_t>(info[2]).ToChecked();
  size_t WidthInBytes = ElementSizeBytes * Nan::To<uint32_t>(info[0]).ToChecked();
  size_t Height = Nan::To<uint32_t>(info[1]).ToChecked();
  CUresult error = cuMemAllocPitch(&(pmem->m_devicePtr), &pPitch, WidthInBytes, Height, ElementSizeBytes);

  result->Set(Nan::New("size").ToLocalChecked(), Nan::New<Integer>(static_cast<uint32_t>(pPitch * Height)));
  result->Set(Nan::New("pitch").ToLocalChecked(), Nan::New<Integer>(static_cast<uint32_t>(pPitch)));
  result->Set(Nan::New("error").ToLocalChecked(), Nan::New<Integer>(error));

  info.GetReturnValue().Set(result);
}

NAN_METHOD(Mem::Free) {
  Mem *pmem = ObjectWrap::Unwrap<Mem>(info.Holder());

  CUresult error = cuMemFree(pmem->m_devicePtr);

  info.GetReturnValue().Set(Nan::New<Integer>(error));
}

NAN_METHOD(Mem::CopyHtoD) {
  Mem *pmem = ObjectWrap::Unwrap<Mem>(info.Holder());

  Local<Object> buf = Nan::To<Object>(info[0]).ToLocalChecked();
  char *phost = Buffer::Data(buf);
  size_t bytes = Buffer::Length(buf);

  bool async = info.Length() >= 2 && info[1]->IsTrue();

  CUresult error;
  if (async) {
    error = cuMemcpyHtoDAsync(pmem->m_devicePtr, phost, bytes, 0);
  } else {
    error = cuMemcpyHtoD(pmem->m_devicePtr, phost, bytes);
  }

  info.GetReturnValue().Set(Nan::New<Integer>(error));
}

NAN_METHOD(Mem::CopyDtoH) {
  Mem *pmem = ObjectWrap::Unwrap<Mem>(info.Holder());

  Local<Object> buf = Nan::To<Object>(info[0]).ToLocalChecked();
  char *phost = Buffer::Data(buf);
  size_t bytes = Buffer::Length(buf);

  bool async = info.Length() >= 2 && info[1]->IsTrue();

  CUresult error;
  if (async) {
    error = cuMemcpyDtoHAsync(phost, pmem->m_devicePtr, bytes, 0);
  } else {
    error = cuMemcpyDtoH(phost, pmem->m_devicePtr, bytes);
  }

  info.GetReturnValue().Set(Nan::New<Integer>(error));
}

NAN_GETTER(Mem::GetDevicePtr) {
  Mem *pmem = ObjectWrap::Unwrap<Mem>(info.Holder());
  Local<Object> buf = Nan::NewBuffer(sizeof(pmem->m_devicePtr)).ToLocalChecked();

  memcpy(Buffer::Data(buf), &(pmem->m_devicePtr), sizeof(pmem->m_devicePtr));

  info.GetReturnValue().Set(buf);
}
