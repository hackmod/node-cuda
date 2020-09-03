#ifndef NODE_CUDA_TYPES_H_
#define NODE_CUDA_TYPES_H_

#include <node.h>
#include "nan.h"
#include <string>
#include <vector>
#include <v8.h>
#include <iostream>
#include <sstream>
#include "common.h"

#include <cuda.h>
#include <nvrtc.h>

using namespace std;
using namespace v8;
using namespace node;

namespace NodeCuda {

extern const char *cu_type_names[];

Nan::Persistent<v8::FunctionTemplate>& prototype(int id);
Nan::Persistent<v8::Function>& constructor(int id);

template <typename T, int id, int err, int cu_release(T), int cu_acquire(T)>
class NodeCUWrapper : public Nan::ObjectWrap {
public:
  NodeCUWrapper() : raw(0) { }

  virtual ~NodeCUWrapper() {
    // std::cout<<"~NodeCUWrapper for elem "<<id<<std::endl;
    assert(release()==CUDA_SUCCESS);
  };

  static NAN_MODULE_INIT(Init) {
    Nan::HandleScope scope;
    Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    Nan::SetPrototypeMethod(tpl, "toString", toString);
    Nan::SetPrototypeMethod(tpl, "getRaw", GetRaw);
    prototype(id).Reset(tpl);
    constructor(id).Reset(Nan::GetFunction(tpl).ToLocalChecked());
  }

  static Local<Object> NewInstance(T raw) {
    Local<v8::Function> ctor = Nan::New(constructor(id));
    Local<Object> obj = Nan::NewInstance(ctor, 0, nullptr).ToLocalChecked();
    //Local<Object> obj = Nan::New<FunctionTemplate>(prototype(id))->InstanceTemplate()->NewInstance();
    Unwrap(obj)->raw = raw;
    return obj;
  }

  static NodeCUWrapper<T, id, err, cu_release, cu_acquire> *Unwrap(Local<Value> value) {
    void *buf = NULL;
    size_t length = 0;
    Local<Object> obj;
    getPtrAndLen(value, buf, length);
    if (buf && length) {
      obj = NewInstance(reinterpret_cast<T>(buf));
    } else if (value->IsObject()) {
      obj = Nan::To<Object>(value).ToLocalChecked();
    } else {
      return nullptr;
    }
    if (!Nan::New(prototype(id))->HasInstance(obj))
      return nullptr;
    return ObjectWrap::Unwrap<NodeCUWrapper<T, id, err, cu_release, cu_acquire> >(obj);
  }

  template <typename A>
  static bool fromJSArray(std::vector<A> & outArr, Local<Array> &arr) {
    for (unsigned int i = 0; i < arr->Length(); ++i) {
      A v = Unwrap(Nan::Get(arr, i).ToLocalChecked());
      if (v == nullptr) {
	     return false;
      }
      outArr.push_back(v);
    }
    return true;
  }

  template <typename U>
  static std::vector<T> toCUArray(std::vector<U> outArr) {
    std::vector<T> out;
    for (unsigned int i = 0; i < outArr.size(); ++i) {
      out.push_back(outArr[i]->getRaw());
    }
    return out;
  }

  T getRaw() const {
    return raw;
  }

  int acquire() const {
    return cu_acquire(raw);
  }

  int release() /*const*/ {
    if(released) return CUDA_SUCCESS;
    // std::cout<<"Release elem "<<id<<std::endl;
    released = true;
    return cu_release(raw);
  }

  static int getErrorCode() {
    return err;
  }

private:
  static NAN_METHOD(New) {
    NodeCUWrapper<T, id, err, cu_release, cu_acquire> *obj = new NodeCUWrapper<T, id, err, cu_release, cu_acquire>();
    obj->Wrap(info.Holder());
    info.GetReturnValue().Set(info.Holder());
  }

  static NAN_METHOD(toString) {
    NodeCUWrapper<T, id, err, cu_release, cu_acquire> *obj = Unwrap(info.Holder());
    std::stringstream ss;
    if (!obj) {
      Nan::TypeError("not a NodeCUWrapper object");
      return;
    }
    ss << "[object " << cu_type_names[id] << "@" << obj->raw << "]";
    info.GetReturnValue().Set(Nan::New<String>(ss.str()).ToLocalChecked());
  }

  static NAN_METHOD(GetRaw) {
    NodeCUWrapper<T, id, err, cu_release, cu_acquire> *obj = Unwrap(info.Holder());
    Local<Object> buf = Nan::NewBuffer(sizeof(obj->raw)).ToLocalChecked();

    memcpy(Buffer::Data(buf), &(obj->raw), sizeof(obj->raw));

    info.GetReturnValue().Set(buf);
  }

  T raw = nullptr;
  bool released = false;
};

template <typename T>
 inline int noop(T _) {
  return 0;
}

#define NOCU_UNWRAP(VAR, TYPE, EXPR) \
  TYPE * VAR = TYPE::Unwrap(EXPR);   \
  if (VAR == NULL) { \
    Nan::ThrowError(JS_STR(NodeCuda::getExceptionMessage(TYPE::getErrorCode()))); \
    return; \
  }

#define NOCU_TO_ARRAY(TO, FROM, TYPE) \
  if (!TYPE::fromJSArray(TO, FROM)) { \
    Nan::ThrowError(JS_STR(NodeCuda::getExceptionMessage(TYPE::getErrorCode()))); \
    return;\
  }

#define NOCU_TO_CU_ARRAY(FROM, TYPE) \
  FROM.size() ? &TYPE::toCUArray(FROM).front() : nullptr

#define NOCU_WRAPPER(name, type, id, err, release, acquire) \
  typedef NodeCUWrapper<type, id, err, release, acquire> name

typedef const unsigned char *cu_program_binary;
typedef const void *cu_mapped_ptr;

NOCU_WRAPPER(NodeCUArray, CUarray, 0, CUDA_ERROR_INVALID_VALUE, noop, noop);
NOCU_WRAPPER(NodeCUContext, CUcontext, 1, CUDA_ERROR_INVALID_CONTEXT, noop, noop);
NOCU_WRAPPER(NodeCUDeviceptr, CUdeviceptr, 2, CUDA_ERROR_INVALID_VALUE, noop, noop);
NOCU_WRAPPER(NodeCUEvent, CUevent, 3, CUDA_ERROR_INVALID_VALUE, noop, noop);
NOCU_WRAPPER(NodeCUFunction, CUfunction, 4, CUDA_ERROR_INVALID_VALUE, noop, noop);
NOCU_WRAPPER(NodeCUModule, CUmodule, 5, CUDA_ERROR_INVALID_VALUE, noop, noop);
NOCU_WRAPPER(NodeCUStream, CUstream, 6, CUDA_ERROR_INVALID_VALUE, noop, noop);
NOCU_WRAPPER(NodeCUNvrtcProgram, nvrtcProgram, 7, CUDA_ERROR_INVALID_VALUE, noop, noop);

#define NOCU_WRAP(T, V) \
  T::NewInstance(V)

namespace Types {

NAN_METHOD(releaseAll);
NAN_MODULE_INIT(init);
}

}  // namespace NodeCuda

#endif
