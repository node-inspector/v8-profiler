#include "cpu_profiler.h"
#include "cpu_profile.h"

namespace nodex {
  using v8::CpuProfile;
  using v8::Handle;
  using v8::Local;
  using v8::Object;
  using v8::Array;
  using v8::String;

  CpuProfiler::CpuProfiler () {}
  CpuProfiler::~CpuProfiler () {}

  void CpuProfiler::Initialize (Local<Object> target) {
    Nan::HandleScope scope;

    Local<Object> cpuProfiler = Nan::New<Object>();
    Local<Array> profiles = Nan::New<Array>();

    Nan::SetMethod(cpuProfiler, "startProfiling", CpuProfiler::StartProfiling);
    Nan::SetMethod(cpuProfiler, "stopProfiling", CpuProfiler::StopProfiling);
    Nan::SetMethod(cpuProfiler, "setSamplingInterval", CpuProfiler::SetSamplingInterval);
    cpuProfiler->Set(Nan::New<String>("profiles").ToLocalChecked(), profiles);

    Profile::profiles.Reset(profiles);
    target->Set(Nan::New<String>("cpu").ToLocalChecked(), cpuProfiler);
  }

  NAN_METHOD(CpuProfiler::StartProfiling) {
    Local<String> title = info[0]->ToString();

#if (NODE_MODULE_VERSION > 0x000B)
    bool recsamples = info[1]->ToBoolean()->Value();
    v8::Isolate::GetCurrent()->GetCpuProfiler()->StartProfiling(title, recsamples);
#else
    v8::CpuProfiler::StartProfiling(title);
#endif
  }

  NAN_METHOD(CpuProfiler::StopProfiling) {
    const CpuProfile* profile;

    Local<String> title = Nan::EmptyString();
    if (info.Length()) {
      if (info[0]->IsString()) {
        title = info[0]->ToString();
      } else if (!info[0]->IsUndefined()) {
        return Nan::ThrowTypeError("Wrong argument [0] type (wait String)");
      }
    }

#if (NODE_MODULE_VERSION > 0x000B)
    profile = v8::Isolate::GetCurrent()->GetCpuProfiler()->StopProfiling(title);
#else
    profile = v8::CpuProfiler::StopProfiling(title);
#endif

    info.GetReturnValue().Set(Profile::New(profile));
  }

  NAN_METHOD(CpuProfiler::SetSamplingInterval) {
#if (NODE_MODULE_VERSION > 0x000B)
    v8::Isolate::GetCurrent()->GetCpuProfiler()->SetSamplingInterval(info[0]->Uint32Value());
#endif
  }
} //namespace nodex
