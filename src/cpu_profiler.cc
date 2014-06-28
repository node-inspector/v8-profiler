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

  void CpuProfiler::Initialize (Handle<Object> target) {
    NanScope();
    
    Local<Object> cpuProfiler = NanNew<Object>();
    Local<Array> profiles = NanNew<Array>();
    
    NODE_SET_METHOD(cpuProfiler, "startProfiling", CpuProfiler::StartProfiling);
    NODE_SET_METHOD(cpuProfiler, "stopProfiling", CpuProfiler::StopProfiling);
    cpuProfiler->Set(NanNew<String>("profiles"), profiles);
    
    NanAssignPersistent(Profile::profiles, profiles);
    target->Set(NanNew<String>("cpu"), cpuProfiler);
  }

  NAN_METHOD(CpuProfiler::StartProfiling) {
    NanScope();
    
    bool recsamples = true;
    Local<String> title = NanNew<String>("");
    if (args.Length()) {
      if (args.Length()>1) {
        if (args[1]->IsBoolean()) {
          recsamples = args[1]->ToBoolean()->Value();
        } else if (!args[1]->IsUndefined()) {
          return NanThrowTypeError("Wrong argument [1] type (wait Boolean)");
        }
        if (args[0]->IsString()) {
          title = args[0]->ToString();
        } else if (!args[0]->IsUndefined()) {
          return NanThrowTypeError("Wrong argument [0] type (wait String)");
        }
      } else {
        if (args[0]->IsString()) {
          title = args[0]->ToString();
        } else if (args[0]->IsBoolean()) {
          recsamples = args[0]->ToBoolean()->Value();
        } else if (!args[0]->IsUndefined()) {
          return NanThrowTypeError("Wrong argument [0] type (wait String or Boolean)");
        }
      }
    }
    
#if (NODE_MODULE_VERSION > 0x000B)
    v8::Isolate::GetCurrent()->GetCpuProfiler()->StartProfiling(title, recsamples);
#else
    v8::CpuProfiler::StartProfiling(title);
#endif

    NanReturnUndefined();
  }

  NAN_METHOD(CpuProfiler::StopProfiling) {
    NanScope();

    const CpuProfile* profile;
    
    Local<String> title = NanNew<String>("");
    if (args.Length()) {
      if (args[0]->IsString()) {
        title = args[0]->ToString();
      } else if (!args[0]->IsUndefined()) {
        return NanThrowTypeError("Wrong argument [0] type (wait String)");
      }
    }
    
#if (NODE_MODULE_VERSION > 0x000B)
    profile = v8::Isolate::GetCurrent()->GetCpuProfiler()->StopProfiling(title);
#else
    profile = v8::CpuProfiler::StopProfiling(title);
#endif
    
    NanReturnValue(Profile::New(profile));
  }
} //namespace nodex
