#include "types.h"
#include "common.h"

namespace NodeCuda {

using namespace v8;

const char *cu_type_names[] = {
  "CUarray",
  "CUcontext",
  "CUdeviceptr",
  "CUevent",
  "CUfunction",
  "CUmodule",
  "CUstream",
};

static Nan::Persistent<FunctionTemplate> prototypes[7];
static Nan::Persistent<Function> constructors[7];

Nan::Persistent<v8::FunctionTemplate>& prototype(int id) {
  return prototypes[id];
}

Nan::Persistent<v8::Function>& constructor(int id) {
  return constructors[id];
}

NAN_METHOD(Types::releaseAll) {
  // force GC to trigger release of lingering OpenCL objects
  static const int idle_time_in_ms = 5;
  Nan::IdleNotification(idle_time_in_ms);

  // be careful with the order of the releases: could segfault if the order is not good
  // on some drivers
  // NodeCUArray::releaseAll();
  // NodeCUContext::releaseAll();
  // NodeCUDeviceptr::releaseAll();
  // NodeCUEvent::releaseAll();
  // NodeCUFunction::releaseAll();
  // NodeCUModule::releaseAll();
  // NodeCUStream::releaseAll();
}

NAN_MODULE_INIT(Types::init)
{
  Nan::SetMethod(target, "releaseAll", releaseAll);

  NodeCUArray::Init(target);
  NodeCUContext::Init(target);
  NodeCUDeviceptr::Init(target);
  NodeCUEvent::Init(target);
  NodeCUFunction::Init(target);
  NodeCUModule::Init(target);
  NodeCUStream::Init(target);
}

}  // namespace NodeCuda
