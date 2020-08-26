#ifndef CTX_HPP
#define CTX_HPP

#include "common.h"
#include "bindings.hpp"
#include "device.hpp"

namespace NodeCuda {

  namespace Ctx {
    NAN_MODULE_INIT(Initialize);
  }

 /*
    private:
      CUcontext m_context;
      CUdevice m_device;

      bool sync_in_progress;

      uv_work_t work_req_;

      static void Process(uv_work_t* work_req);
      static void After(uv_work_t* work_req, int status);
  };
  */

}

#endif
