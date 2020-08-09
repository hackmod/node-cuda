#include "ctx.hpp"
#include "device.hpp"

using namespace NodeCuda;

Nan::Persistent<FunctionTemplate> Ctx::constructor;

NAN_MODULE_INIT(Ctx::Initialize) {
  Local<FunctionTemplate> t = Nan::New<FunctionTemplate>(Ctx::New);
  t->InstanceTemplate()->SetInternalFieldCount(1);
  t->SetClassName(Nan::New("CudaCtx").ToLocalChecked());

  Nan::SetPrototypeMethod(t, "destroy", Ctx::Destroy);
  Nan::SetPrototypeMethod(t, "pushCurrent", Ctx::PushCurrent);
  Nan::SetPrototypeMethod(t, "popCurrent", Ctx::PopCurrent);
  Nan::SetPrototypeMethod(t, "setCurrent", Ctx::SetCurrent);
  Nan::SetPrototypeMethod(t, "getCurrent", Ctx::GetCurrent);
  Nan::SetPrototypeMethod(t, "synchronize", Ctx::Synchronize);
  Nan::SetAccessor(t->InstanceTemplate(), Nan::New("apiVersion").ToLocalChecked(), Ctx::GetApiVersion);

  target->Set(Nan::New("Ctx").ToLocalChecked(), t->GetFunction());
  constructor.Reset(t);
}

NAN_METHOD(Ctx::New) {
  Ctx *pctx = new Ctx();
  pctx->Wrap(info.Holder());

  unsigned int flags = Nan::To<uint32_t>(info[0]).ToChecked();
  pctx->m_device = ObjectWrap::Unwrap<Device>(Nan::To<Object>(info[1]).ToLocalChecked())->m_device;

  cuCtxCreate(&(pctx->m_context), flags, pctx->m_device);

  info.GetReturnValue().Set(info.Holder());
}

NAN_METHOD(Ctx::Destroy) {
  Ctx *pctx = ObjectWrap::Unwrap<Ctx>(info.Holder());

  CUresult error = cuCtxDestroy(pctx->m_context);
  info.GetReturnValue().Set(Nan::New<Integer>(error));
}

NAN_METHOD(Ctx::PushCurrent) {
  Ctx *pctx = ObjectWrap::Unwrap<Ctx>(info.Holder());

  CUresult error = cuCtxPushCurrent(pctx->m_context);
  info.GetReturnValue().Set(Nan::New<Integer>(error));
}

NAN_METHOD(Ctx::PopCurrent) {
  Ctx *pctx = ObjectWrap::Unwrap<Ctx>(info.Holder());

  CUresult error = cuCtxPopCurrent(&(pctx->m_context));
  info.GetReturnValue().Set(Nan::New<Integer>(error));
}

NAN_METHOD(Ctx::SetCurrent) {
  Ctx *pctx = ObjectWrap::Unwrap<Ctx>(info.Holder());

  CUresult error = cuCtxSetCurrent(pctx->m_context);
  info.GetReturnValue().Set(Nan::New<Integer>(error));
}

NAN_METHOD(Ctx::GetCurrent) {
  Ctx *pctx = ObjectWrap::Unwrap<Ctx>(info.Holder());

  CUresult error = cuCtxGetCurrent(&(pctx->m_context));
  info.GetReturnValue().Set(Nan::New<Integer>(error));
}

struct SynchronizeParams {
  Ctx *ctx;
  CUresult error;
  Nan::Callback *cb;
};

NAN_METHOD(Ctx::Synchronize) {
  if (info.Length() >= 1 && info[0]->IsFunction()) {
    // Asynchronous

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
}

void Ctx::Process(uv_work_t* work_req) {
  SynchronizeParams *params = static_cast<SynchronizeParams*>(work_req->data);

  params->error = cuCtxPushCurrent(params->ctx->m_context);
  if (params->error) return;

  params->error = cuCtxSynchronize();
  if (params->error) return;

  params->error = cuCtxPopCurrent(NULL);
}

void Ctx::After(uv_work_t* work_req, int status) {
  Nan::HandleScope scope;

  assert(status == 0);
  SynchronizeParams *params = static_cast<SynchronizeParams*>(work_req->data);

  params->ctx->Unref();
  params->ctx->sync_in_progress = false;

  cuCtxPushCurrent(params->ctx->m_context);

  Local<Value> argv[1];
  argv[0] = Nan::New<Integer>(params->error);

  Nan::TryCatch try_catch;
  params->cb->Call(1, argv);
  if (try_catch.HasCaught()) FatalException(try_catch);

  uv_unref((uv_handle_t*) work_req);
  delete params;
}

NAN_GETTER(Ctx::GetApiVersion) {
  Ctx *pctx = ObjectWrap::Unwrap<Ctx>(info.Holder());

  unsigned int version;
  CUresult error = cuCtxGetApiVersion(pctx->m_context, &version);

  info.GetReturnValue().Set(Nan::New<Integer>(version));
}
