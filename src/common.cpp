#include "common.h"
#include <iostream>

namespace NodeCuda {

class ScopedArrayBufferContents {
 public:
  explicit ScopedArrayBufferContents(const v8::ArrayBuffer::Contents& contents)
      : contents_(contents) {}
  ~ScopedArrayBufferContents() { free(contents_.Data()); }
  void* Data() const { return contents_.Data(); }
  size_t ByteLength() const { return contents_.ByteLength(); }

 private:
  const v8::ArrayBuffer::Contents contents_;
};

// TODO replace TypedArray with node::Buffer or v8::ArrayBuffer (same thing)
void getPtrAndLen(const Local<Value> value, void* &ptr, size_t &len)
{
  Nan::HandleScope scope;

  ptr=nullptr;
  len=0;
  if(!value->IsUndefined() && !value->IsNull()) {
    if(value->IsArray()) {
      // JS Array
      // std::cout<<"[getPtrAndLen] JS array"<<std::endl;
      // Local<Array> arr=value.As<Array>();
      // len=arr->Length();
      // can't access a pointer to JS Array data anymore...
    }
    else if(value->IsArrayBuffer()) {
      // std::cout<<"[getPtrAndLen] ArrayBuffer"<<std::endl;
      Local<Object> obj = Nan::To<Object>(value).ToLocalChecked();
      Local<ArrayBuffer> ta = obj.As<ArrayBuffer>();
      len=ta->ByteLength();
      ptr=ta->GetContents().Data();
    }
    else if(value->IsUint8Array()) {
      // WARNING node::Buffer is an augmented Uint8Array
      // std::cout<<"[getPtrAndLen] Uint8Array"<<std::endl;
      Local<Object> obj = Nan::To<Object>(value).ToLocalChecked();
      Local<Uint8Array> ui = obj.As<Uint8Array>();
      ArrayBuffer::Contents ab_c = ui->Buffer()->GetContents();
      len=ui->ByteLength();
      ptr=static_cast<char*>(ab_c.Data()) + ui->ByteOffset();
    }
    else if(value->IsTypedArray()) {
      // std::cout<<"[getPtrAndLen] TypedArray"<<std::endl;
      Local<Object> obj = Nan::To<Object>(value).ToLocalChecked();
      Local<TypedArray> ta = obj.As<TypedArray>();
      len=ta->ByteLength();
      ptr=static_cast<char*>(ta->Buffer()->GetContents().Data()) + ta->ByteOffset();
    }
    // else if(value->IsObject()) {
      // shouldn't be called...
      // std::cout<<"[getPtrAndLen] object"<<std::endl;
      /*Local<Object> obj = Nan::To<Object>(value).ToLocalChecked();
      String::Utf8Value name(obj->GetConstructorName());
      std::cout<<"  object name: "<<*name<<std::endl;
      if(!strcmp("Buffer",*name)) {
        std::cout<<"[getPtrAndLen] node::Buffer"<<std::endl;
        // node::Buffer
        ptr=node::Buffer::Data(obj);
        len=(int) node::Buffer::Length(obj);
      }*/
    // }
  }
}

const char* getExceptionMessage(const int code) {
  switch (code) {
    case CUDA_SUCCESS:                             return "Success";
    case CUDA_ERROR_INVALID_VALUE:                 return "CUDA_ERROR_INVALID_VALUE";
    case CUDA_ERROR_OUT_OF_MEMORY:                 return "CUDA_ERROR_OUT_OF_MEMORY";
    case CUDA_ERROR_NOT_INITIALIZED:               return "CUDA_ERROR_NOT_INITIALIZED";
    case CUDA_ERROR_DEINITIALIZED:                 return "CUDA_ERROR_DEINITIALIZED";
    case CUDA_ERROR_PROFILER_DISABLED:             return "CUDA_ERROR_PROFILER_DISABLED";
    case CUDA_ERROR_PROFILER_NOT_INITIALIZED:      return "CUDA_ERROR_PROFILER_NOT_INITIALIZED";
    case CUDA_ERROR_PROFILER_ALREADY_STARTED:      return "CUDA_ERROR_PROFILER_ALREADY_STARTED";
    case CUDA_ERROR_PROFILER_ALREADY_STOPPED:      return "CUDA_ERROR_PROFILER_ALREADY_STOPPED";
    case CUDA_ERROR_NO_DEVICE:                     return "CUDA_ERROR_NO_DEVICE";
    case CUDA_ERROR_INVALID_DEVICE:                return "CUDA_ERROR_INVALID_DEVICE";
    case CUDA_ERROR_INVALID_IMAGE:                 return "CUDA_ERROR_INVALID_IMAGE";
    case CUDA_ERROR_INVALID_CONTEXT:               return "CUDA_ERROR_INVALID_CONTEXT";
    case CUDA_ERROR_CONTEXT_ALREADY_CURRENT:       return "CUDA_ERROR_CONTEXT_ALREADY_CURRENT";
    case CUDA_ERROR_MAP_FAILED:                    return "CUDA_ERROR_MAP_FAILED";
    case CUDA_ERROR_UNMAP_FAILED:                  return "CUDA_ERROR_UNMAP_FAILED";
    case CUDA_ERROR_ARRAY_IS_MAPPED:               return "CUDA_ERROR_ARRAY_IS_MAPPED";
    case CUDA_ERROR_ALREADY_MAPPED:                return "CUDA_ERROR_ALREADY_MAPPED";
    case CUDA_ERROR_NO_BINARY_FOR_GPU:             return "CUDA_ERROR_NO_BINARY_FOR_GPU";
    case CUDA_ERROR_ALREADY_ACQUIRED:              return "CUDA_ERROR_ALREADY_ACQUIRED";
    case CUDA_ERROR_NOT_MAPPED:                    return "CUDA_ERROR_NOT_MAPPED";
    case CUDA_ERROR_NOT_MAPPED_AS_ARRAY:           return "CUDA_ERROR_NOT_MAPPED_AS_ARRAY";
    case CUDA_ERROR_NOT_MAPPED_AS_POINTER:         return "CUDA_ERROR_NOT_MAPPED_AS_POINTER";
    case CUDA_ERROR_ECC_UNCORRECTABLE:             return "CUDA_ERROR_ECC_UNCORRECTABLE";
    case CUDA_ERROR_UNSUPPORTED_LIMIT:             return "CUDA_ERROR_UNSUPPORTED_LIMIT";
    case CUDA_ERROR_CONTEXT_ALREADY_IN_USE:        return "CUDA_ERROR_CONTEXT_ALREADY_IN_USE";
    case CUDA_ERROR_PEER_ACCESS_UNSUPPORTED:       return "CUDA_ERROR_PEER_ACCESS_UNSUPPORTED";
    case CUDA_ERROR_INVALID_PTX:                   return "CUDA_ERROR_INVALID_PTX";
    case CUDA_ERROR_INVALID_GRAPHICS_CONTEXT:      return "CUDA_ERROR_INVALID_GRAPHICS_CONTEXT";
    case CUDA_ERROR_NVLINK_UNCORRECTABLE:          return "CUDA_ERROR_NVLINK_UNCORRECTABLE";
    case CUDA_ERROR_JIT_COMPILER_NOT_FOUND:        return "CUDA_ERROR_JIT_COMPILER_NOT_FOUND";
    case CUDA_ERROR_INVALID_SOURCE:                return "CUDA_ERROR_INVALID_SOURCE";
    case CUDA_ERROR_FILE_NOT_FOUND:                return "CUDA_ERROR_FILE_NOT_FOUND";
    case CUDA_ERROR_SHARED_OBJECT_SYMBOL_NOT_FOUND:return "CUDA_ERROR_SHARED_OBJECT_SYMBOL_NOT_FOUND";
    case CUDA_ERROR_SHARED_OBJECT_INIT_FAILED:     return "CUDA_ERROR_SHARED_OBJECT_INIT_FAILED";
    case CUDA_ERROR_OPERATING_SYSTEM:              return "CUDA_ERROR_OPERATING_SYSTEM";
    case CUDA_ERROR_INVALID_HANDLE:                return "CUDA_ERROR_INVALID_HANDLE";
#if __CUDA_API_VERSION >= 10010
    case CUDA_ERROR_ILLEGAL_STATE:                 return "CUDA_ERROR_ILLEGAL_STATE";
#endif
    case CUDA_ERROR_NOT_FOUND:                     return "CUDA_ERROR_NOT_FOUND";
    case CUDA_ERROR_NOT_READY:                     return "CUDA_ERROR_NOT_READY";
    case CUDA_ERROR_ILLEGAL_ADDRESS:               return "CUDA_ERROR_ILLEGAL_ADDRESS";
    case CUDA_ERROR_LAUNCH_OUT_OF_RESOURCES:       return "CUDA_ERROR_LAUNCH_OUT_OF_RESOURCES";
    case CUDA_ERROR_LAUNCH_TIMEOUT:                return "CUDA_ERROR_LAUNCH_TIMEOUT";
    case CUDA_ERROR_LAUNCH_INCOMPATIBLE_TEXTURING: return "CUDA_ERROR_LAUNCH_INCOMPATIBLE_TEXTURING";
    case CUDA_ERROR_PEER_ACCESS_ALREADY_ENABLED:   return "CUDA_ERROR_PEER_ACCESS_ALREADY_ENABLED";
    case CUDA_ERROR_PEER_ACCESS_NOT_ENABLED:       return "CUDA_ERROR_PEER_ACCESS_NOT_ENABLED";
    case CUDA_ERROR_PRIMARY_CONTEXT_ACTIVE:        return "CUDA_ERROR_PRIMARY_CONTEXT_ACTIVE";
    case CUDA_ERROR_CONTEXT_IS_DESTROYED:          return "CUDA_ERROR_CONTEXT_IS_DESTROYED";
    case CUDA_ERROR_ASSERT:                        return "CUDA_ERROR_ASSERT";
    case CUDA_ERROR_TOO_MANY_PEERS:                return "CUDA_ERROR_TOO_MANY_PEERS";
    case CUDA_ERROR_HOST_MEMORY_ALREADY_REGISTERED:return "CUDA_ERROR_HOST_MEMORY_ALREADY_REGISTERED";
    case CUDA_ERROR_HOST_MEMORY_NOT_REGISTERED:    return "CUDA_ERROR_HOST_MEMORY_NOT_REGISTERED";
    case CUDA_ERROR_HARDWARE_STACK_ERROR:          return "CUDA_ERROR_HARDWARE_STACK_ERROR";
    case CUDA_ERROR_ILLEGAL_INSTRUCTION:           return "CUDA_ERROR_ILLEGAL_INSTRUCTION";
    case CUDA_ERROR_MISALIGNED_ADDRESS:            return "CUDA_ERROR_MISALIGNED_ADDRESS";
    case CUDA_ERROR_INVALID_ADDRESS_SPACE:         return "CUDA_ERROR_INVALID_ADDRESS_SPACE";
    case CUDA_ERROR_INVALID_PC:                    return "CUDA_ERROR_INVALID_PC";
    case CUDA_ERROR_LAUNCH_FAILED:                 return "CUDA_ERROR_LAUNCH_FAILED";
    case CUDA_ERROR_COOPERATIVE_LAUNCH_TOO_LARGE:  return "CUDA_ERROR_COOPERATIVE_LAUNCH_TOO_LARGE";
    case CUDA_ERROR_NOT_PERMITTED:                 return "CUDA_ERROR_NOT_PERMITTED";
    case CUDA_ERROR_NOT_SUPPORTED:                 return "CUDA_ERROR_NOT_SUPPORTED";
#if __CUDA_API_VERSION >= 10010
    case CUDA_ERROR_SYSTEM_NOT_READY:              return "CUDA_ERROR_SYSTEM_NOT_READY";
    case CUDA_ERROR_SYSTEM_DRIVER_MISMATCH:        return "CUDA_ERROR_SYSTEM_DRIVER_MISMATCH";
    case CUDA_ERROR_COMPAT_NOT_SUPPORTED_ON_DEVICE:return "CUDA_ERROR_COMPAT_NOT_SUPPORTED_ON_DEVICE";
    case CUDA_ERROR_STREAM_CAPTURE_UNSUPPORTED:    return "CUDA_ERROR_STREAM_CAPTURE_UNSUPPORTED";
    case CUDA_ERROR_STREAM_CAPTURE_INVALIDATED:    return "CUDA_ERROR_STREAM_CAPTURE_INVALIDATED";
    case CUDA_ERROR_STREAM_CAPTURE_MERGE:          return "CUDA_ERROR_STREAM_CAPTURE_MERGE";
    case CUDA_ERROR_STREAM_CAPTURE_UNMATCHED:      return "CUDA_ERROR_STREAM_CAPTURE_UNMATCHED";
    case CUDA_ERROR_STREAM_CAPTURE_UNJOINED:       return "CUDA_ERROR_STREAM_CAPTURE_UNJOINED";
    case CUDA_ERROR_STREAM_CAPTURE_ISOLATION:      return "CUDA_ERROR_STREAM_CAPTURE_ISOLATION";
    case CUDA_ERROR_STREAM_CAPTURE_IMPLICIT:       return "CUDA_ERROR_STREAM_CAPTURE_IMPLICIT";
    case CUDA_ERROR_CAPTURED_EVENT:                return "CUDA_ERROR_CAPTURED_EVENT";
    case CUDA_ERROR_STREAM_CAPTURE_WRONG_THREAD:   return "CUDA_ERROR_STREAM_CAPTURE_WRONG_THREAD";
#endif
    case CUDA_ERROR_UNKNOWN:                       return "CUDA_ERROR_UNKNOWN";
    default:                                       return "Unknown Error";
  }
}

}

// namespace NodeCuda
