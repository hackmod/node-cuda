#ifndef NVRTC_HPP
#define NVRTC_HPP

#include <nvrtc.h>
#include "bindings.hpp"

#define NVRTC_CHECK_ERR(ret) do {                                           \
  nvrtcResult result = (nvrtcResult)ret;                                    \
  if (result != NVRTC_SUCCESS) {                                            \
    std::stringstream s;                                                    \
    s << "NVRTC error at " << __FUNCTION__ << " (" __FILE__ ":" << __LINE__ \
      << ") " << funcs._nvrtcGetErrorString(result);                        \
    return Nan::ThrowError(JS_STR(s.str()));                                \
  }                                                                         \
} while(0)

namespace NodeCuda {

  namespace Nvrtc {
    NAN_METHOD(NvrtcLoad);
    NAN_MODULE_INIT(Initialize);
    NAN_METHOD(NvrtcVersion);
    NAN_METHOD(CreateProgram);
    NAN_METHOD(CompileProgram);
    NAN_METHOD(DestroyProgram);
    NAN_METHOD(GetPTXSize);
    NAN_METHOD(GetPTX);
    NAN_METHOD(GetProgramLogSize);
    NAN_METHOD(GetProgramLog);
  } // namespace Nvrtc

  struct nvrtcFunctions {
    nvrtcResult (*_nvrtcVersion)(int*, int*) = nullptr;
    nvrtcResult (*_nvrtcCreateProgram)(nvrtcProgram*, const char*, const char*, int, const char**, const char**) = nullptr;
    nvrtcResult (*_nvrtcCompileProgram)(nvrtcProgram, int, const char **) = nullptr;
    nvrtcResult (*_nvrtcDestroyProgram)(nvrtcProgram*) = nullptr;
    nvrtcResult (*_nvrtcGetPTXSize)(nvrtcProgram, size_t*) = nullptr;
    nvrtcResult (*_nvrtcGetPTX)(nvrtcProgram, char*) = nullptr;
    nvrtcResult (*_nvrtcGetProgramLogSize)(nvrtcProgram, size_t*) = nullptr;
    nvrtcResult (*_nvrtcGetProgramLog)(nvrtcProgram, char*) = nullptr;
    nvrtcResult (*_nvrtcAddNameExpression)(nvrtcProgram, const char*) = nullptr;
    const char* (*_nvrtcGetErrorString)(nvrtcResult) = nullptr;
  };

}

#endif
