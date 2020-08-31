#include <stdio.h>
#include "mem.hpp"
#include "types.h"
#include "common.h"

#include <algorithm>
#include <cstring>
#include <string>
#include <tuple>
#include <unordered_map>

namespace NodeCuda {

// from node-opencl project
//
// caches CUDA type name to conversion function mapping in a hash table
// (unordered_map) for fast retrieval. This is much faster than the previous
// approach of checking each possible type with strcmp in a huge if-else
class PrimitiveTypeMapCache {
 private:
  /// Type of the conversion function
  typedef std::function<std::tuple<size_t, void*, int32_t>(const Local<Value>&)> func_t;

  // map of conversion functions
  std::unordered_map<std::string, func_t> m_converters;

 public:
  PrimitiveTypeMapCache() {
    // if we create the TypeMap as a static function member, the constructor
    // is guarantueed to be called by only one thread (see C++11 standard sec 6.7)
    // while all other threads wait for completion. Thus no manual synchronization
    // is needed for the initialization.

    // create all type mappers

    /* convert primitive types */

    #define CONVERT_NUMBER(NAME, TYPE, PRED, CONV) {                           \
      func_t f = [](const Local<Value>& val)                                   \
        -> std::tuple<size_t, void*, int32_t> {                                \
        if (!val->PRED()) {                                                    \
         return std::tuple<size_t, void*, int32_t>(0, NULL, CUDA_ERROR_INVALID_VALUE); \
        }                                                                      \
        void* ptr_data = new TYPE;                                             \
        size_t ptr_size = sizeof(TYPE);                                        \
        *((TYPE *)ptr_data) = (TYPE) Nan::To<CONV>(val).FromJust();            \
        return std::tuple<size_t, void*, int32_t>(ptr_size, ptr_data, 0);      \
      };                                                                       \
      m_converters[NAME] = f;                                                  \
    }

    CONVERT_NUMBER("char", char, IsInt32, int32_t);
    CONVERT_NUMBER("uchar", unsigned char, IsInt32, uint32_t);
    CONVERT_NUMBER("short", short, IsInt32, int32_t);
    CONVERT_NUMBER("ushort", unsigned short, IsInt32, uint32_t);
    CONVERT_NUMBER("int", int32_t , IsInt32, int32_t);
    CONVERT_NUMBER("uint", uint32_t, IsUint32, uint32_t);
    CONVERT_NUMBER("long", int64_t, IsNumber, int64_t);
    CONVERT_NUMBER("ulong", int64_t, IsNumber, int64_t);
    CONVERT_NUMBER("float", float, IsNumber, double);
    CONVERT_NUMBER("double", double, IsNumber, double);

    #undef CONVERT_NUMBER

    /* convert vector types (e.g. float4, int16, etc) */

    #define CONVERT_VECT(NAME, TYPE, I, PRED, COND) {                          \
      func_t f = [](const Local<Value>& val)                                   \
          -> std::tuple<size_t, void*, int32_t> {                              \
        if (!val->IsArray()) {                                                 \
          return std::tuple<size_t, void*, int32_t>(0, NULL, CUDA_ERROR_INVALID_VALUE); \
        }                                                                      \
        Local<Array> arr = Local<Array>::Cast(val);                            \
        if (arr->Length() != I) {                                              \
          return std::tuple<size_t, void*, int32_t>(0, NULL, CUDA_ERROR_INVALID_VALUE); \
        }                                                                      \
        TYPE * vvc = new TYPE[I];                                              \
        size_t ptr_size = sizeof(TYPE) * I;                                    \
        void* ptr_data = vvc;                                                  \
        for (unsigned int i = 0; i < I; ++i) {                                 \
          if (!Nan::Get(arr, i).ToLocalChecked()->PRED()) {                    \
            return std::tuple<size_t, void*, int32_t>(0, NULL, CUDA_ERROR_INVALID_VALUE); \
          }                                                                    \
          vvc[i] = (TYPE) Nan::To<COND>(Nan::Get(arr, i).ToLocalChecked()).FromJust();  \
        }                                                                      \
        return std::tuple<size_t, void*, int32_t>(ptr_size, ptr_data, 0);      \
      };                                                                       \
      m_converters[NAME #I ] = f;                                              \
    }

    #define CONVERT_VECTS(NAME, TYPE, PRED, COND) \
      CONVERT_VECT(NAME, TYPE, 2, PRED, COND); \
      CONVERT_VECT(NAME, TYPE, 3, PRED, COND); \
      CONVERT_VECT(NAME, TYPE, 4, PRED, COND); \
      CONVERT_VECT(NAME, TYPE, 8, PRED, COND); \
      CONVERT_VECT(NAME, TYPE, 16, PRED, COND);

    CONVERT_VECTS("char", char, IsInt32, int32_t);
    CONVERT_VECTS("uchar", unsigned char, IsInt32, int32_t);
    CONVERT_VECTS("short", short, IsInt32, int32_t);
    CONVERT_VECTS("ushort", unsigned short, IsInt32, int32_t);
    CONVERT_VECTS("int", int32_t, IsInt32, int32_t);
    CONVERT_VECTS("uint", uint32_t, IsUint32, uint32_t);
    CONVERT_VECTS("long", int64_t, IsNumber, int64_t);
    CONVERT_VECTS("ulong", int64_t, IsNumber, int64_t);
    CONVERT_VECTS("float", float, IsNumber, double);
    CONVERT_VECTS("double", double, IsNumber, double);

    #undef CONVERT_VECT
    #undef CONVERT_VECTS

    // add boolean conversion
    m_converters["bool"] = [](const Local<Value>& val) {
      size_t ptr_size = sizeof(bool);
      void* ptr_data = new bool;
      *((bool *)ptr_data) = Nan::To<bool>(val).FromJust() ? 1 : 0;
      return std::tuple<size_t, void*, int32_t>(ptr_size, ptr_data, 0);
    };
  }

  /// Returns wheather the type given is in the map, i.e. if it is a
  /// primitive type
  bool hasType(const std::string& name) {
    return m_converters.find(name) != m_converters.end();
  }

  // Converts the given JS value to the CUDA type given by the `name`
  // parameter and returns the result as a pair of the size of the converted
  // value and the pointer as `void*`. The caller is responsible
  // for deleting the pointer after use.
  std::tuple<size_t, void*, int32_t> convert(const std::string& name,
      const Local<Value>& val) {
    assert(this->hasType(name));
    // call conversion function and return size of argument and pointer
    return m_converters[name](val);
  }
};

NAN_METHOD(MemAlloc) {
  size_t bytesize = Nan::To<uint32_t>(info[0]).ToChecked();
  CUdeviceptr devicePtr;
  CUresult error = cuMemAlloc(&devicePtr, bytesize);

  CHECK_ERR(error);
  info.GetReturnValue().Set(NOCU_WRAP(NodeCUDeviceptr, devicePtr));
}

void callback(char *Data, void*hint) {
  return;
}

NAN_METHOD(MemAllocHost) {
  REQ_ARGS(1);

  void *pp;
  size_t bytesize = Nan::To<int64_t>(info[0]).ToChecked();
  CUresult error = cuMemAllocHost((void**)&pp, bytesize);

  CHECK_ERR(error);

  info.GetReturnValue().Set(Nan::NewBuffer(static_cast<char*>(pp), bytesize, callback, 0).ToLocalChecked());
}

NAN_METHOD(MemHostAlloc) {
  REQ_ARGS(1);

  void *pp;
  size_t bytesize = Nan::To<uint32_t>(info[0]).ToChecked();
  unsigned int flags = 0;
  if (info.Length() > 1) {
    flags = Nan::To<int64_t>(info[1]).ToChecked();
  }

  CUresult error = cuMemHostAlloc((void**)&pp, bytesize, flags);

  CHECK_ERR(error);
  info.GetReturnValue().Set(Nan::NewBuffer(static_cast<char*>(pp), bytesize, callback, 0).ToLocalChecked());
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
  static PrimitiveTypeMapCache type_converter;

  REQ_ARGS(2);

  NOCU_UNWRAP(devicePtr, NodeCUDeviceptr, info[0]);

  void* data;
  size_t bytes;
  CUresult error;

  std::string type_name;
  // read argument 2 as the name of the data type
  if (info[1]->IsString()) {
    Local<String> s = Nan::To<String>(info[1]).ToLocalChecked();
    Nan::Utf8String tname(s);
    const char* tname_c = *tname;

    size_t len = tname.length();
    type_name.resize(len);
    std::copy(tname_c, tname_c + len, type_name.begin());

    bool async = info.Length() >= 4 && info[3]->IsTrue();

    if ('&' == type_name[type_name.length() - 1]) {
      // simply copy address of CUdeviceptr
      NOCU_UNWRAP(devPtr, NodeCUDeviceptr, info[2]);
      CUdeviceptr dptr = devPtr->getRaw();

      if (async) {
        error = cuMemcpyHtoDAsync(devicePtr->getRaw(), (void**)&dptr, sizeof(CUdeviceptr), 0);
      } else {
        error = cuMemcpyHtoD(devicePtr->getRaw(), (void**)&dptr, sizeof(CUdeviceptr));
      }
      info.GetReturnValue().Set(Nan::New<Integer>(error));
      return;
    } else if (type_converter.hasType(type_name)) {
      // convert primitive types using the conversion
      // function map (indexed by type name)
      int32_t err;
      std::tie(bytes, data, err) = type_converter.convert(type_name, info[2]);

      CHECK_ERR(err);

      if (async) {
        error = cuMemcpyHtoDAsync(devicePtr->getRaw(), data, bytes, 0);
      } else {
        error = cuMemcpyHtoD(devicePtr->getRaw(), data, bytes);
      }
      free(data);

      info.GetReturnValue().Set(Nan::New<Integer>(error));
      return;
    }
  }

  // always buffer
  Local<Object> buf = Nan::To<Object>(info[1]).ToLocalChecked();
  data = Buffer::Data(buf);
  bytes = Buffer::Length(buf);

  bool async = info.Length() >= 3 && info[2]->IsTrue();

  if (async) {
    error = cuMemcpyHtoDAsync(devicePtr->getRaw(), data, bytes, 0);
  } else {
    error = cuMemcpyHtoD(devicePtr->getRaw(), data, bytes);
  }

  info.GetReturnValue().Set(Nan::New<Integer>(error));
  return;
}

NAN_METHOD(MemcpyDtoH) {
  REQ_ARGS(2);

  Local<Object> buf = Nan::To<Object>(info[0]).ToLocalChecked();

  NOCU_UNWRAP(devicePtr, NodeCUDeviceptr, info[1]);

  void *phost;
  size_t bytes;
  if (Buffer::HasInstance(buf)) {
    phost = Buffer::Data(buf);
    bytes = Buffer::Length(buf);
  } else {
    return Nan::ThrowError("Argument 1 should be a buffer object.");
  }

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
  Nan::SetMethod(target, "memAllocHost", MemAllocHost);
  Nan::SetMethod(target, "memHostAlloc", MemHostAlloc);
  Nan::SetMethod(target, "memAllocPitch", MemAllocPitch);
  Nan::SetMethod(target, "memFree", MemFree);
  Nan::SetMethod(target, "memcpyHtoD", MemcpyHtoD);
  Nan::SetMethod(target, "memcpyDtoH", MemcpyDtoH);
  Nan::SetMethod(target, "memcpyAtoA", MemcpyAtoA);
  Nan::SetMethod(target, "memcpyDtoD", MemcpyDtoD);
}

}  // namespace NodeCuda
