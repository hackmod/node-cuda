#ifndef MODULE_HPP
#define MODULE_HPP

#include <cuda.h>
#include "bindings.hpp"

namespace NodeCuda {

  namespace Module {
    NAN_MODULE_INIT(Initialize);
  } // namespace Module

}

#endif
