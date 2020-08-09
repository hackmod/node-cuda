#ifndef MEM_HPP
#define MEM_HPP

#include <cuda.h>
#include "bindings.hpp"
#include "function.hpp"

namespace NodeCuda {

  class Mem : public ObjectWrap {
    public:
      static NAN_MODULE_INIT(Initialize);

    protected:
      static Nan::Persistent<FunctionTemplate> constructor;

      static NAN_METHOD(Alloc);
      static NAN_METHOD(AllocPitch);
      static NAN_METHOD(Free);
      static NAN_METHOD(CopyHtoD);
      static NAN_METHOD(CopyDtoH);

      static NAN_GETTER(GetDevicePtr);

      Mem() : ObjectWrap(), m_devicePtr(0) {}

      ~Mem() {}

    private:
      static NAN_METHOD(New);

      CUdeviceptr m_devicePtr;

      friend class NodeCuda::Function;
  };

}

#endif
