#include "ctx.hpp"
#include "device.hpp"
#include "types.h"

using namespace NodeCuda;

NAN_METHOD(CtxCreate) {
  REQ_ARGS(2);

  unsigned int flags = Nan::To<uint32_t>(info[0]).ToChecked();
  CUdevice device = Nan::To<int32_t>(info[1]).ToChecked();

  CUcontext context;
  cuCtxCreate(&context, flags, device);

  info.GetReturnValue().Set(NOCU_WRAP(NodeCUContext, context));
}

NAN_METHOD(CtxDestroy) {
  REQ_ARGS(1);

  NOCU_UNWRAP(context, NodeCUContext, info[0]);

  CUresult error = cuCtxDestroy(context->getRaw());

  CHECK_ERR(error);
  info.GetReturnValue().Set(Nan::New<Integer>(error));
}

NAN_METHOD(CtxPushCurrent) {
  REQ_ARGS(1);

  NOCU_UNWRAP(context, NodeCUContext, info[0]);

  CUresult error = cuCtxPushCurrent(context->getRaw());

  CHECK_ERR(error);
  info.GetReturnValue().Set(Nan::New<Integer>(error));
}

NAN_METHOD(CtxPopCurrent) {
  CUcontext context;
  CUresult error = cuCtxPopCurrent(&context);

  CHECK_ERR(error);
  info.GetReturnValue().Set(NOCU_WRAP(NodeCUContext, context));
}

NAN_METHOD(CtxSetCurrent) {
  REQ_ARGS(1);

  NOCU_UNWRAP(context, NodeCUContext, info[0]);

  CUresult error = cuCtxSetCurrent(context->getRaw());

  CHECK_ERR(error);
  info.GetReturnValue().Set(Nan::New<Integer>(error));
}

NAN_METHOD(CtxGetCurrent) {
  CUcontext context;
  CUresult error = cuCtxGetCurrent(&context);

  CHECK_ERR(error);
  info.GetReturnValue().Set(NOCU_WRAP(NodeCUContext, context));
}

struct SynchronizeParams {
  //Ctx *ctx;
  CUcontext *ctx;
  CUresult error;
  Nan::Callback *cb;
};

NAN_METHOD(CtxSynchronize) {
  CUresult error = cuCtxSynchronize();
  info.GetReturnValue().Set(Nan::New<Integer>(error));

  /*
  if (info.Length() >= 1 && info[0]->IsFunction()) {
    // Asynchronous

    NOCU_UNWRAP(ctx, NodeCUContext, info[1]);

    Ctx *ctx = ObjectWrap::Unwrap<Ctx>(info.Holder());
    if (ctx->sync_in_progress) {
      info.GetReturnValue().Set(Nan::New<Integer>(-1));
    }

    SynchronizeParams *params = new SynchronizeParams();
    params->ctx = ctx;
    params->cb = new Nan::Callback(info[0].As<Function>());

    cuCtxPopCurrent(NULL);

    // build up the work request
    uv_work_t* work_req = new uv_work_t;
    work_req->data = params;

    uv_queue_work(uv_default_loop(),
        work_req,
        Process,
        After);
    uv_ref((uv_handle_t*) &work_req);

    ctx->Ref();
    ctx->sync_in_progress = true;

    info.GetReturnValue().Set(Nan::Undefined());
  } else {
    // Synchronous
    CUresult error = cuCtxSynchronize();
    info.GetReturnValue().Set(Nan::New<Integer>(error));
  }
  */
}

/*
void Process(uv_work_t* work_req) {
  SynchronizeParams *params = static_cast<SynchronizeParams*>(work_req->data);

  params->error = cuCtxPushCurrent(params->ctx->m_context);
  if (params->error) return;

  params->error = cuCtxSynchronize();
  if (params->error) return;

  params->error = cuCtxPopCurrent(NULL);
}

void After(uv_work_t* work_req, int status) {
  Nan::HandleScope scope;

  assert(status == 0);
  SynchronizeParams *params = static_cast<SynchronizeParams*>(work_req->data);

  params->ctx->Unref();
  params->ctx->sync_in_progress = false;

  cuCtxPushCurrent(params->ctx->m_context);

  Local<Value> argv[1];
  argv[0] = Nan::New<Integer>(params->error);

  Nan::TryCatch try_catch;
  params->cb->Call(1, argv, NULL);
  if (try_catch.HasCaught()) FatalException(try_catch);

  uv_unref((uv_handle_t*) work_req);
  delete params;
}
*/

NAN_METHOD(CtxGetApiVersion) {
  REQ_ARGS(1);

  NOCU_UNWRAP(context, NodeCUContext, info[0]);

  unsigned int version;
  CUresult error = cuCtxGetApiVersion(context->getRaw(), &version);

  CHECK_ERR(error);
  info.GetReturnValue().Set(Nan::New<Integer>(version));
}

NAN_MODULE_INIT(Ctx::Initialize) {
  Nan::SetMethod(target, "ctxCreate", CtxCreate);
  Nan::SetMethod(target, "ctxDestroy", CtxDestroy);
  Nan::SetMethod(target, "ctxPushCurrent", CtxPushCurrent);
  Nan::SetMethod(target, "ctxPopCurrent", CtxPopCurrent);
  Nan::SetMethod(target, "ctxSetCurrent", CtxSetCurrent);
  Nan::SetMethod(target, "ctxGetCurrent", CtxGetCurrent);
  Nan::SetMethod(target, "ctxSynchronize", CtxSynchronize);
  Nan::SetMethod(target, "ctxGetApiVersion", CtxGetApiVersion);
}
