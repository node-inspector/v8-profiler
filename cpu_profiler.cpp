#include "cpu_profiler.h"
#include "profile.h"

namespace nodex {
    Persistent<ObjectTemplate> CpuProfiler::cpu_profiler_template_;

    void CpuProfiler::Initialize(Handle<ObjectTemplate> target) {
        HandleScope scope;

        cpu_profiler_template_ = Persistent<ObjectTemplate>::New(ObjectTemplate::New());

        NODE_SET_PROTOTYPE_METHOD2(cpu_profiler_template_, "getProfilesCount", CpuProfiler::GetProfilesCount);
        NODE_SET_PROTOTYPE_METHOD2(cpu_profiler_template_, "getProfile", CpuProfiler::GetProfile);
        NODE_SET_PROTOTYPE_METHOD2(cpu_profiler_template_, "findProfile", CpuProfiler::FindProfile);
        NODE_SET_PROTOTYPE_METHOD2(cpu_profiler_template_, "startProfiling", CpuProfiler::StartProfiling);
        NODE_SET_PROTOTYPE_METHOD2(cpu_profiler_template_, "stopProfiling", CpuProfiler::StopProfiling);
        NODE_SET_PROTOTYPE_METHOD2(cpu_profiler_template_, "deleteAllProfiles", CpuProfiler::DeleteAllProfiles);

        target->Set(String::NewSymbol("cpuProfiler"), cpu_profiler_template_);
    }

    CpuProfiler::CpuProfiler() {}
    CpuProfiler::~CpuProfiler() {}

    Handle<Value> CpuProfiler::GetProfilesCount(const Arguments& args) {
        HandleScope scope;
        return scope.Close(Integer::New(v8::CpuProfiler::GetProfilesCount()));
    }

    Handle<Value> CpuProfiler::GetProfile(const Arguments& args) {
        HandleScope scope;
        if (args.Length() < 1) {
            return ThrowException(Exception::Error(String::New("No index specified")));
        } else if (!args[0]->IsInt32()) {
            return ThrowException(Exception::TypeError(String::New("Argument must be an integer")));
        }
        int32_t index = args[0]->Int32Value();
        const CpuProfile* profile = v8::CpuProfiler::GetProfile(index);
        return scope.Close(Profile::New(profile));
    }

    Handle<Value> CpuProfiler::FindProfile(const Arguments& args) {
        HandleScope scope;
        if (args.Length() < 1) {
            return ThrowException(Exception::Error(String::New("No index specified")));
        } else if (!args[0]->IsInt32()) {
            return ThrowException(Exception::TypeError(String::New("Argument must be an integer")));
        }
        uint32_t uid = args[0]->Uint32Value();
        const CpuProfile* profile = v8::CpuProfiler::FindProfile(uid);
        return scope.Close(Profile::New(profile));
    }

    Handle<Value> CpuProfiler::StartProfiling(const Arguments& args) {
        HandleScope scope;
        Local<String> title = args.Length() > 0 ? args[0]->ToString() : String::New("");
        v8::CpuProfiler::StartProfiling(title);
        return Undefined();
    }

    Handle<Value> CpuProfiler::StopProfiling(const Arguments& args) {
        HandleScope scope;
        Local<String> title = args.Length() > 0 ? args[0]->ToString() : String::New("");
        const CpuProfile* profile = v8::CpuProfiler::StopProfiling(title);
        return scope.Close(Profile::New(profile));
    }

    Handle<Value> CpuProfiler::DeleteAllProfiles(const Arguments& args) {
        v8::CpuProfiler::DeleteAllProfiles();
        return Undefined();
    }
} //namespace nodex
