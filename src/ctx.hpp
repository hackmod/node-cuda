#ifndef CTX_HPP
#define CTX_HPP

#include <cuda.h>
#include "bindings.hpp"
#include "device.hpp"

namespace NodeCuda {

  class Ctx : public ObjectWrap {
    public:
      static NAN_MODULE_INIT(Initialize);

    protected:
      static Nan::Persistent<FunctionTemplate> constructor;

      static NAN_METHOD(New);
      static NAN_METHOD(Destroy);
      static NAN_METHOD(PushCurrent);
      static NAN_METHOD(PopCurrent);
      static NAN_METHOD(SetCurrent);
      static NAN_METHOD(GetCurrent);
      static NAN_METHOD(Synchronize);
      static NAN_GETTER(GetApiVersion);

      Ctx() : ObjectWrap(), m_context(NULL), m_device(0), sync_in_progress(false) {}

      ~Ctx () {}

    private:
      CUcontext m_context;
      CUdevice m_device;

      bool sync_in_progress;

      uv_work_t work_req_;

      static void Process(uv_work_t* work_req);
      static void After(uv_work_t* work_req, int status);
  };

}

#endif
