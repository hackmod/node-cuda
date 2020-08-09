#include "module.hpp"
#include "function.hpp"

using namespace NodeCuda;

Nan::Persistent<FunctionTemplate> NodeCuda::Module::constructor;

NAN_MODULE_INIT(NodeCuda::Module::Initialize) {
  Local<FunctionTemplate> t = Nan::New<FunctionTemplate>(NodeCuda::Module::New);
  t->InstanceTemplate()->SetInternalFieldCount(1);
  t->SetClassName(Nan::New("CudaModule").ToLocalChecked());

  // Module objects can only be created by load functions
  Nan::SetMethod(target, "moduleLoad", NodeCuda::Module::Load);

  Nan::SetPrototypeMethod(t, "getFunction", NodeCuda::Module::GetFunction);

  target->Set(Nan::New("Module").ToLocalChecked(), t->GetFunction());
  NodeCuda::Module::constructor.Reset(t);
}

NAN_METHOD(NodeCuda::Module::New) {
  NodeCuda::Module *pmodule = new NodeCuda::Module();
  pmodule->Wrap(info.Holder());

  info.GetReturnValue().Set(info.Holder());
}

NAN_METHOD(NodeCuda::Module::Load) {
  Local<Object> result = Nan::New<FunctionTemplate>(NodeCuda::Module::constructor)->InstanceTemplate()->NewInstance();
  NodeCuda::Module *pmodule = new NodeCuda::Module();
  pmodule->Wrap(result);
  //NodeCuda::Module *pmodule = ObjectWrap::Unwrap<NodeCuda::Module>(result);

  Nan::Utf8String fname(info[0]);
  CUresult error = cuModuleLoad(&(pmodule->m_module), *fname);

  result->Set(Nan::New("fname").ToLocalChecked(), Nan::To<String>(info[0]).ToLocalChecked());
  result->Set(Nan::New("error").ToLocalChecked(), Nan::New<Integer>(error));

  info.GetReturnValue().Set(result);
}

NAN_METHOD(NodeCuda::Module::GetFunction) {
  Local<Object> result = Nan::New<FunctionTemplate>(NodeCuda::Function::constructor)->InstanceTemplate()->NewInstance();
  NodeCuda::Module *pmodule = ObjectWrap::Unwrap<NodeCuda::Module>(info.Holder());
  NodeCuda::Function *pfunction = new NodeCuda::Function();
  pfunction->Wrap(result);
  //NodeCuda::Function *pfunction = ObjectWrap::Unwrap<NodeCuda::Function>(result);

  Nan::Utf8String name(info[0]);
  CUresult error = cuModuleGetFunction(&pfunction->m_function, pmodule->m_module, *name);

  result->Set(Nan::New("name").ToLocalChecked(), Nan::To<String>(info[0]).ToLocalChecked());
  result->Set(Nan::New("error").ToLocalChecked(), Nan::New<Integer>(error));

  info.GetReturnValue().Set(result);
}

