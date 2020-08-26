#ifndef CUDA_COMMON_H_
#define CUDA_COMMON_H_

// Node includes
#include <nan.h>
#include <string>
#include <memory>
#include <vector>
#ifdef LOGGING
#include <iostream>
#endif

using namespace std;
using namespace v8;
using namespace Nan;

#include <cuda.h>

namespace {
#define JS_STR(...) Nan::New<v8::String>(__VA_ARGS__).ToLocalChecked()
#define JS_INT(val) Nan::New<v8::Integer>(static_cast<unsigned int>(val))
#define JS_NUM(val) Nan::New<v8::Number>(val)
//#define JS_BOOL(val) Nan::New<v8::Boolean>(val)
#define JS_RETHROW(tc) Nan::New<v8::Local<v8::Value> >(tc.Exception());

#define CHECK_ERR(ret) do {                                                 \
  CUresult result = (CUresult)ret;                                          \
  if (result != CUDA_SUCCESS) {                                             \
    std::stringstream ss;                                                   \
    const char* msg;                                                        \
    cuGetErrorName(result, &msg);                                           \
    ss << "CUDA error at " << __FUNCTION__ << " (" __FILE__ ":" << __LINE__ \
       << ") " << msg;                                                      \
    return Nan::ThrowError(JS_STR(ss.str()));                               \
  }                                                                         \
} while (0)

#define CUDA_SAFE_CALL(call) do {                                           \
  cudaError_t err = call;                                                   \
  if (cudaSuccess != err) {                                                 \
    std::stringstream ss;                                                   \
    ss << "CUDA error at " << __FUNCTION__ << " (" __FILE__ ":" << __LINE__ \
       << ") " << cudaGetErrorString(err);                                  \
    return Nan::ThrowError(JS_STR(ss.str()));                               \
  }                                                                         \
} while (0)

#define CU_SAFE_CALL(call) do {                                             \
  CUresult result = call;                                                   \
  if (result != CUDA_SUCCESS) {                                             \
    std::stringstream ss;                                                   \
    const char* msg;                                                        \
    cuGetErrorName(result, &msg);                                           \
    ss << "CUDA error at " << __FUNCTION__ << " (" __LINE__ ":" << __LINE__ \
       << ") " << msg;                                                      \
    return Nan::ThrowError(JS_STR(ss.str()));                               \
  }                                                                         \
while (0)

#define THROW_ERR(code) { int _err=(code); \
  return Nan::ThrowError(JS_STR(NodeCuda::getExceptionMessage(_err)));   \
}

#define REQ_ARGS(N)                                                    \
  if (info.Length() < (N)) {                                           \
    Nan::ThrowTypeError("Expected " #N " arguments");                  \
    return;                                                            \
  }

#define REQ_STR_ARG(I, VAR)                                            \
  if (info.Length() <= (I) || !info[I]->IsString())  {                 \
    Nan::ThrowTypeError("Argument " #I " must be a string");           \
    return;                                                            \
  }                                                                    \
  Nan::Utf8String VAR(info[I]);

#define REQ_ARRAY_ARG(I, VAR) \
  if (!info[I]->IsArray()) { \
    Nan::ThrowTypeError("Argument " #I " must be an array");           \
    return;                                                            \
  }                                                                    \
  Local<Array> VAR = Local<Array>::Cast(info[I])

} // namespace

namespace NodeCuda {

#define ARG_EXISTS(nth) \
  (info.Length() >= nth + 1 && !info[nth]->IsNull() && !info[nth]->IsUndefined())


void getPtrAndLen(const Local<Value> value, void* &ptr, size_t &len);

//template<typename CU_TYPE>
//void getValuesFromArray(const Local<Array>& arr, std::vector<CU_TYPE>& vals)
//{
//  size_t num=arr->Length();
//  if(!num) {
//    vals.clear();
//    return;
//  }
//
//  vals.reserve(num);
//  for(size_t i=0;i<num;i++)
//    vals.push_back()
//    vals.push_back(Unwrap<CU_TYPE>(arr->Get(i)));
//}

const char* getExceptionMessage(const int code);

} // namespace NodeCuda

#endif // CUDA_COMMON_H_
