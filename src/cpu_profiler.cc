#include "cpu_profiler.h"
#include "profile.h"

namespace nodex {
    Persistent<ObjectTemplate> CpuProfiler::cpu_profiler_template_;

    void CpuProfiler::Initialize(Handle<Object> target) {
        NanScope();

        Local<ObjectTemplate> tpl = NanNewLocal<ObjectTemplate>(ObjectTemplate::New());
        NanAssignPersistent(ObjectTemplate, cpu_profiler_template_, tpl);
        tpl->SetInternalFieldCount(1);

        Local<Object> cpuProfilerObj = tpl->NewInstance();

        NODE_SET_METHOD(cpuProfilerObj, "getProfilesCount", CpuProfiler::GetProfilesCount);
        NODE_SET_METHOD(cpuProfilerObj, "getProfile", CpuProfiler::GetProfile);
        NODE_SET_METHOD(cpuProfilerObj, "findProfile", CpuProfiler::FindProfile);
        NODE_SET_METHOD(cpuProfilerObj, "startProfiling", CpuProfiler::StartProfiling);
        NODE_SET_METHOD(cpuProfilerObj, "stopProfiling", CpuProfiler::StopProfiling);
        NODE_SET_METHOD(cpuProfilerObj, "deleteAllProfiles", CpuProfiler::DeleteAllProfiles);

        target->Set(String::NewSymbol("cpuProfiler"), cpuProfilerObj);
    }

    CpuProfiler::CpuProfiler() {}
    CpuProfiler::~CpuProfiler() {}

    NAN_METHOD(CpuProfiler::GetProfilesCount) {
        NanScope();
#if (NODE_MODULE_VERSION > 0x000B)
        int ret = nan_isolate->GetCpuProfiler()->GetProfileCount();
#else
        int ret = v8::CpuProfiler::GetProfilesCount();
#endif
        NanReturnValue(Integer::New(ret));
    }

    NAN_METHOD(CpuProfiler::GetProfile) {
        NanScope();
        if (args.Length() < 1) {
            return NanThrowError("No index specified");
        } else if (!args[0]->IsInt32()) {
            return NanThrowError("Argument must be an integer");
        }
        int32_t index = args[0]->Int32Value();
#if (NODE_MODULE_VERSION > 0x000B)
        const CpuProfile* profile = nan_isolate->GetCpuProfiler()->GetCpuProfile(index);
#else
        const CpuProfile* profile = v8::CpuProfiler::GetProfile(index);
#endif
        NanReturnValue(Profile::New(profile));
    }

    NAN_METHOD(CpuProfiler::FindProfile) {
        NanScope();
        if (args.Length() < 1) {
            return NanThrowError("No index specified");
        } else if (!args[0]->IsInt32()) {
            return NanThrowError("Argument must be an integer");
        }
        uint32_t uid = args[0]->Uint32Value();
        const CpuProfile* profile;

#if (NODE_MODULE_VERSION > 0x000B)
        bool notFound = true;
        int count = nan_isolate->GetCpuProfiler()->GetProfileCount();
        for (int32_t index = 1; index < count; index++) {
            profile = nan_isolate->GetCpuProfiler()->GetCpuProfile(index);
            if (profile->GetUid() == uid) {
                notFound = false;
                break;
            }
        }
        if (notFound) {
            NanReturnNull();
        }
#else
        profile = v8::CpuProfiler::FindProfile(uid);
#endif
        NanReturnValue(Profile::New(profile));
    }

    NAN_METHOD(CpuProfiler::StartProfiling) {
        NanScope();
        Local<String> title = args.Length() > 0 ? args[0]->ToString() : String::New("");
#if (NODE_MODULE_VERSION > 0x000B)
        nan_isolate->GetCpuProfiler()->StartCpuProfiling(title);
#else
        v8::CpuProfiler::StartProfiling(title);
#endif
        NanReturnUndefined();
    }

    NAN_METHOD(CpuProfiler::StopProfiling) {
        NanScope();
        Local<String> title = args.Length() > 0 ? args[0]->ToString() : String::New("");
#if (NODE_MODULE_VERSION > 0x000B)
        const CpuProfile* profile = nan_isolate->GetCpuProfiler()->StopCpuProfiling(title);
#else
        const CpuProfile* profile = v8::CpuProfiler::StopProfiling(title);
#endif
        NanReturnValue(Profile::New(profile));
    }

    NAN_METHOD(CpuProfiler::DeleteAllProfiles) {
#if (NODE_MODULE_VERSION > 0x000B)
        nan_isolate->GetCpuProfiler()->DeleteAllCpuProfiles();
#else
        v8::CpuProfiler::DeleteAllProfiles();
#endif
        NanReturnUndefined();
    }
} //namespace nodex
