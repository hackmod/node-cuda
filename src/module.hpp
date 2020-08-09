#ifndef MODULE_HPP
#define MODULE_HPP

#include <cuda.h>
#include "bindings.hpp"

namespace NodeCuda {

  class Module : public ObjectWrap {
    public:
      static NAN_MODULE_INIT(Initialize);
      static NAN_METHOD(GetFunction);

    protected:
      static Nan::Persistent<FunctionTemplate> constructor;

      static NAN_METHOD(Load);

      Module() : ObjectWrap(), m_module(0) {}

      ~Module() {}

    private:
      static NAN_METHOD(New);

      CUmodule m_module;
  };

}

#endif
