#include "nvrtc.hpp"
#include "types.h"

#ifdef _WIN32
#else
#include <dlfcn.h>
#endif

namespace NodeCuda {
namespace Nvrtc {
  static nvrtcFunctions funcs;

NAN_METHOD(NvrtcLoad) {
  if (funcs._nvrtcVersion != nullptr) {
    return Nan::ThrowError("NvrtcLoad loaded already");
  }

  REQ_ARGS(1);

  Nan::Utf8String nvrtc(info[0]);
  void *handle = dlopen(*nvrtc, RTLD_LAZY);

  if (handle == NULL) {
    return Nan::ThrowError("Argument 1 should be the name of a valid dynamic lib.");
  }

  funcs._nvrtcVersion = (nvrtcResult (*)(int*, int*))dlsym(handle, "nvrtcVersion");
  funcs._nvrtcCreateProgram = (nvrtcResult (*)(nvrtcProgram*, const char*, const char*, int, const char**, const char**))dlsym(handle, "nvrtcCreateProgram");
  funcs._nvrtcCompileProgram = (nvrtcResult (*)(nvrtcProgram, int, const char**))dlsym(handle, "nvrtcCompileProgram");
  funcs._nvrtcGetProgramLogSize = (nvrtcResult (*)(nvrtcProgram, size_t*))dlsym(handle, "nvrtcGetProgramLogSize");
  funcs._nvrtcGetProgramLog = (nvrtcResult (*)(nvrtcProgram, char*))dlsym(handle, "nvrtcGetProgramLog");
  funcs._nvrtcGetPTXSize = (nvrtcResult (*)(nvrtcProgram, size_t*))dlsym(handle, "nvrtcGetPTXSize");
  funcs._nvrtcGetPTX = (nvrtcResult (*)(nvrtcProgram, char*))dlsym(handle, "nvrtcGetPTX");
  funcs._nvrtcAddNameExpression = (nvrtcResult (*)(nvrtcProgram, const char*))dlsym(handle, "nvrtcAddNameExpression");
  funcs._nvrtcDestroyProgram = (nvrtcResult (*)(nvrtcProgram*))dlsym(handle, "nvrtcDestroyProgram");
  funcs._nvrtcGetErrorString = (const char* (*)(nvrtcResult))dlsym(handle, "nvrtcGetErrorString");

  //dlclose(handle);

  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(NvrtcVersion) {
  int major = 0, minor = 0;
  funcs._nvrtcVersion(&major, &minor);

  Local<Object> result = Nan::New<Object>();
  result->Set(Nan::New("major").ToLocalChecked(), Nan::New<Integer>(major));
  result->Set(Nan::New("minor").ToLocalChecked(), Nan::New<Integer>(minor));
  info.GetReturnValue().Set(result);
}

NAN_METHOD(CreateProgram) {
  REQ_ARGS(1);

  if (!info[0]->IsString()) {
    return Nan::ThrowError("Argument 2 should be a string object.");
  }
  Nan::Utf8String input(info[0]);

  Nan::Utf8String * name = nullptr;

  if (ARG_EXISTS(1)) {
    if (!info[1]->IsString()) {
      return Nan::ThrowError("Argument 2 should be a string object.");
    }
    name = new Nan::Utf8String(info[1]);
  }

  int numOptions = 0;
  const char** header_contents = NULL;
  const char** headers = NULL;
  if (info.Length() > 3 && info[2]->IsArray() && info[3]->IsArray()) {
    // get headers and header_contents from array
    Local<Array> header_contents_arr = Local<Array>::Cast(info[2]);
    Local<Array> headers_arr = Local<Array>::Cast(info[3]);

    headers = new const char* [headers_arr->Length()];
    header_contents = new const char* [headers_arr->Length()];
    for (unsigned int j = 0; j < headers_arr->Length(); j++) {
      Nan::Utf8String *opt = new Nan::Utf8String(Nan::Get(headers_arr, j).ToLocalChecked());
      headers[j] = **opt;
      numOptions++;
    }
    for (unsigned int j = 0; j < headers_arr->Length(); j++) {
      Nan::Utf8String *opt = new Nan::Utf8String(Nan::Get(header_contents_arr, j).ToLocalChecked());
      header_contents[j] = **opt;
    }
  } else if (info.Length() > 2 && info[2]->IsObject()) {
    // get headers and header_contents from the given object like as {"stdio.h":"\n",...}
    Local<Object> headers_obj = Nan::To<Object>(info[2]).ToLocalChecked();
    Local<Array> props = headers_obj->GetOwnPropertyNames();

    headers = new const char* [props->Length()];
    header_contents = new const char* [props->Length()];
    for (unsigned int j = 0; j < props->Length(); j++) {
      const Local<Value> key = Nan::Get(props, j).ToLocalChecked();
      Nan::Utf8String *opt = new Nan::Utf8String(key);
      headers[j] = **opt;
      opt = new Nan::Utf8String(Nan::Get(headers_obj, key).ToLocalChecked());
      header_contents[j] = **opt;
      numOptions++;
    }
  }

  nvrtcProgram prog;
  nvrtcResult error = funcs._nvrtcCreateProgram(&prog, *input, name != nullptr ? **name : NULL, numOptions, header_contents, headers);

  NVRTC_CHECK_ERR(error);
  info.GetReturnValue().Set(NOCU_WRAP(NodeCUNvrtcProgram, prog));
}

NAN_METHOD(CompileProgram) {
  REQ_ARGS(1);

  NOCU_UNWRAP(prog, NodeCUNvrtcProgram, info[0]);

  int numOptions = 0;
  const char** options = NULL;

  if (info.Length() > 1 && info[1]->IsArray()) {
    Local<Array> arr = Local<Array>::Cast(info[1]);
    options = new const char* [arr->Length()];
    for (unsigned int j = 0; j < arr->Length(); j++) {
      Nan::Utf8String *opt = new Nan::Utf8String(Nan::Get(arr, j).ToLocalChecked());
      options[j] = **opt;
      numOptions++;
    }
  }

  nvrtcResult error = funcs._nvrtcCompileProgram(prog->getRaw(), numOptions, options);

  info.GetReturnValue().Set(Nan::New<Integer>(error));
}

NAN_METHOD(AddNameExpression) {
  REQ_ARGS(2);

  NOCU_UNWRAP(prog, NodeCUNvrtcProgram, info[0]);

  if (!info[1]->IsString()) {
    return Nan::ThrowError("Argument 2 should be a string object.");
  }
  Nan::Utf8String name(info[1]);

  nvrtcResult error = funcs._nvrtcAddNameExpression(prog->getRaw(), *name);

  NVRTC_CHECK_ERR(error);
  info.GetReturnValue().Set(Nan::New<Integer>(error));
}

NAN_METHOD(GetProgramLogSize) {
  REQ_ARGS(1);

  NOCU_UNWRAP(prog, NodeCUNvrtcProgram, info[0]);

  size_t sz = 0;

  nvrtcResult error = funcs._nvrtcGetProgramLogSize(prog->getRaw(), &sz);

  double value = sz;

  NVRTC_CHECK_ERR(error);
  info.GetReturnValue().Set(Nan::New(value));
}

NAN_METHOD(GetProgramLog) {
  REQ_ARGS(1);

  NOCU_UNWRAP(prog, NodeCUNvrtcProgram, info[0]);

  size_t sz = 0;
  nvrtcResult error = funcs._nvrtcGetProgramLogSize(prog->getRaw(), &sz);
  NVRTC_CHECK_ERR(error);

  char *log = new char[sz];
  error = funcs._nvrtcGetProgramLog(prog->getRaw(), log);

  NVRTC_CHECK_ERR(error);
  info.GetReturnValue().Set(JS_STR(log));
}

NAN_METHOD(DestroyProgram) {
  REQ_ARGS(1);

  NOCU_UNWRAP(prog, NodeCUNvrtcProgram, info[0]);

  nvrtcProgram program = prog->getRaw();

  nvrtcResult error = funcs._nvrtcDestroyProgram(&program);

  NVRTC_CHECK_ERR(error);
  info.GetReturnValue().Set(Nan::New<Integer>(error));
}

NAN_METHOD(GetPTXSize) {
  REQ_ARGS(1);

  NOCU_UNWRAP(prog, NodeCUNvrtcProgram, info[0]);

  size_t sz = 0;
  nvrtcResult error = funcs._nvrtcGetPTXSize(prog->getRaw(), &sz);
  NVRTC_CHECK_ERR(error);

  double value = sz;
  info.GetReturnValue().Set(Nan::New(value));
}

NAN_METHOD(GetPTX) {
  REQ_ARGS(1);

  NOCU_UNWRAP(prog, NodeCUNvrtcProgram, info[0]);

  size_t sz = 0;
  nvrtcResult error = funcs._nvrtcGetPTXSize(prog->getRaw(), &sz);
  NVRTC_CHECK_ERR(error);

  char *ptx = new char[sz];
  error = funcs._nvrtcGetPTX(prog->getRaw(), ptx);
  NVRTC_CHECK_ERR(error);

  info.GetReturnValue().Set(JS_STR(ptx));
}

NAN_MODULE_INIT(Initialize) {
  Nan::SetMethod(target, "nvrtcLoad", NvrtcLoad);
  Nan::SetMethod(target, "nvrtcVersion", NvrtcVersion);
  Nan::SetMethod(target, "nvrtcCompileProgram", CompileProgram);
  Nan::SetMethod(target, "nvrtcCreateProgram", CreateProgram);
  Nan::SetMethod(target, "nvrtcDestroyProgram", DestroyProgram);
  Nan::SetMethod(target, "nvrtcGetPTX", GetPTX);
  Nan::SetMethod(target, "nvrtcGetPTXSize", GetPTXSize);
  Nan::SetMethod(target, "nvrtcGetProgramLog", GetProgramLog);
  Nan::SetMethod(target, "nvrtcGetProgramLogSize", GetProgramLogSize);
  Nan::SetMethod(target, "nvrtcAddNameExpression", AddNameExpression);
}

}  // namespace Nvrtc

}  // namespace NodeCuda
