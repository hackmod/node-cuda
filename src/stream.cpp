#include "stream.hpp"
#include "types.h"

namespace NodeCuda {

NAN_METHOD(StreamCreate) {
  REQ_ARGS(1);

  unsigned int flags = Nan::To<int32_t>(info[0]).ToChecked();

  CUstream stream;
  CUresult error = cuStreamCreate(&stream, flags);
  CHECK_ERR(error);

  info.GetReturnValue().Set(NOCU_WRAP(NodeCUStream, stream));
}

NAN_METHOD(StreamDestroy) {
  REQ_ARGS(1);

  NOCU_UNWRAP(stream, NodeCUStream, info[0]);

  CUresult error = cuStreamDestroy(stream->getRaw());
  CHECK_ERR(error);

  info.GetReturnValue().Set(Nan::New<Integer>(error));
}

NAN_METHOD(StreamSynchronize) {
  REQ_ARGS(1);

  NOCU_UNWRAP(stream, NodeCUStream, info[0]);

  CUresult error = cuStreamSynchronize(stream->getRaw());
  CHECK_ERR(error);

  info.GetReturnValue().Set(Nan::New<Integer>(error));
}

NAN_MODULE_INIT(Stream::Initialize) {
  Nan::SetMethod(target, "streamCreate", StreamCreate);
  Nan::SetMethod(target, "streamDestroy", StreamDestroy);
  Nan::SetMethod(target, "streamSynchronize", StreamSynchronize);
}

}  // namespace NodeCuda
