#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <cuda.h>
#include "bindings.hpp"

namespace NodeCuda {

  class Device : public ObjectWrap {
    public:
      static NAN_MODULE_INIT(Initialize);

    protected:
      static Nan::Persistent<FunctionTemplate> constructor;

      static NAN_METHOD(New);
      static NAN_GETTER(GetComputeCapability);
      static NAN_GETTER(GetName);
      static NAN_GETTER(GetTotalMem);

      // TODO: cuDeviceGetAttribute
      // TODO: cuDeviceGetProperties

      Device() : ObjectWrap(), m_device(0) {}

      ~Device() {}

    private:
      CUdevice m_device;

      friend class Ctx;
  };

}

#endif
