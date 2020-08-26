#include <cstring>
#include <stdio.h>
#include "mem.hpp"
#include "types.h"
#include "common.h"

using namespace NodeCuda;

NAN_METHOD(MemAlloc) {
  size_t bytesize = Nan::To<uint32_t>(info[0]).ToChecked();
  CUdeviceptr devicePtr;
  CUresult error = cuMemAlloc(&devicePtr, bytesize);

  CHECK_ERR(error);
  info.GetReturnValue().Set(NOCU_WRAP(NodeCUDeviceptr, devicePtr));
}

NAN_METHOD(MemAllocPitch) {
  size_t pPitch;
  unsigned int ElementSizeBytes = Nan::To<uint32_t>(info[2]).ToChecked();
  size_t WidthInBytes = ElementSizeBytes * Nan::To<uint32_t>(info[0]).ToChecked();
  size_t Height = Nan::To<uint32_t>(info[1]).ToChecked();

  CUdeviceptr devicePtr;
  CUresult error = cuMemAllocPitch(&devicePtr, &pPitch, WidthInBytes, Height, ElementSizeBytes);

  CHECK_ERR(error);
  info.GetReturnValue().Set(NOCU_WRAP(NodeCUDeviceptr, devicePtr));
}

NAN_METHOD(MemFree) {
  REQ_ARGS(1);

  CUdeviceptr devicePtr = Nan::To<uint32_t>(info[0]).ToChecked();

  CUresult error = cuMemFree(devicePtr);

  info.GetReturnValue().Set(Nan::New<Integer>(error));
}

NAN_METHOD(MemcpyHtoD) {
  REQ_ARGS(2);

  NOCU_UNWRAP(devicePtr, NodeCUDeviceptr, info[0]);

  Local<Object> buf = Nan::To<Object>(info[1]).ToLocalChecked();
  char *phost = Buffer::Data(buf);
  size_t bytes = Buffer::Length(buf);

  bool async = info.Length() >= 3 && info[2]->IsTrue();

  CUresult error;
  if (async) {
    error = cuMemcpyHtoDAsync(devicePtr->getRaw(), phost, bytes, 0);
  } else {
    error = cuMemcpyHtoD(devicePtr->getRaw(), phost, bytes);
  }

  info.GetReturnValue().Set(Nan::New<Integer>(error));
}

NAN_METHOD(MemcpyDtoH) {
  REQ_ARGS(2);

  Local<Object> buf = Nan::To<Object>(info[0]).ToLocalChecked();

  NOCU_UNWRAP(devicePtr, NodeCUDeviceptr, info[1]);

  char *phost = Buffer::Data(buf);
  size_t bytes = Buffer::Length(buf);

  bool async = info.Length() >= 3 && info[2]->IsTrue();

  CUresult error;
  if (async) {
    error = cuMemcpyDtoHAsync(phost, devicePtr->getRaw(), bytes, 0);
  } else {
    error = cuMemcpyDtoH(phost, devicePtr->getRaw(), bytes);
  }

  info.GetReturnValue().Set(Nan::New<Integer>(error));
}

NAN_METHOD(MemcpyDtoD) {
  REQ_ARGS(3);

  NOCU_UNWRAP(dst, NodeCUDeviceptr, info[0]);
  NOCU_UNWRAP(src, NodeCUDeviceptr, info[1]);

  size_t size = (size_t)Nan::To<int64_t>(info[2]).FromJust();

  bool async = info.Length() >= 4 && info[3]->IsTrue();

  CUresult error;
  if (async) {
    error = cuMemcpyDtoDAsync(dst->getRaw(), src->getRaw(), size, 0);
  } else {
    error = cuMemcpyDtoD(dst->getRaw(), src->getRaw(), size);
  }

  info.GetReturnValue().Set(Nan::New<Integer>(error));
}

NAN_METHOD(MemcpyAtoA) {
  REQ_ARGS(5);

  NOCU_UNWRAP(dst, NodeCUArray, info[0]);
  size_t dstOffset = (size_t)Nan::To<int64_t>(info[1]).FromJust();

  NOCU_UNWRAP(src, NodeCUArray, info[2]);
  size_t srcOffset = (size_t)Nan::To<int64_t>(info[3]).FromJust();

  size_t byteCount = (size_t)Nan::To<int64_t>(info[4]).FromJust();

  CUresult error = cuMemcpyAtoA(dst->getRaw(), dstOffset, src->getRaw(), srcOffset, byteCount);

  info.GetReturnValue().Set(Nan::New<Integer>(error));
}

NAN_MODULE_INIT(Mem::Initialize) {
  // NodeCUdevicePtr objects can only be created by allocation functions
  Nan::SetMethod(target, "memAlloc", MemAlloc);
  Nan::SetMethod(target, "memAllocPitch", MemAllocPitch);
  Nan::SetMethod(target, "memFree", MemFree);
  Nan::SetMethod(target, "memcpyHtoD", MemcpyHtoD);
  Nan::SetMethod(target, "memcpyDtoH", MemcpyDtoH);
  Nan::SetMethod(target, "memcpyAtoA", MemcpyAtoA);
  Nan::SetMethod(target, "memcpyDtoD", MemcpyDtoD);
}
