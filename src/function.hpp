#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include <cuda.h>
#include "bindings.hpp"
#include "module.hpp"

namespace NodeCuda {

  class Function : public ObjectWrap {
    public:
      static NAN_MODULE_INIT(Initialize);

    protected:
      static Nan::Persistent<FunctionTemplate> constructor;

      static NAN_METHOD(LaunchKernel);

      Function() : ObjectWrap(), m_function(0) {}

      ~Function() {}

    private:
      static NAN_METHOD(New);

      CUfunction m_function;

      friend NAN_METHOD(Module::GetFunction);
  };

}

#endif
