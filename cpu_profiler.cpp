#include "cpu_profiler.h"
#include "profiler.h"
#include <cstdlib>

namespace nodex {
    Persistent<ObjectTemplate> CpuProfiler::cpu_profiler_template_;

	// Added this one -cat
	static void parseTree(Handle<String> &snapshot, const CpuProfileNode *node) {

		char leading[512];

		String::Utf8Value functionName8(node->GetFunctionName());
		const char *functionName = *functionName8 ? *functionName8 : "<string conversion failed>";
		String::Utf8Value scriptName8(node->GetScriptResourceName());
		const char *scriptName = *scriptName8 ? *scriptName8 : "<string conversion failed>";

		snprintf(leading, sizeof(leading), "{\"functionName\":\"%s\",\"scriptName\":\"%s\",\"url\":\"%s\",\"lineNumber\":%d,\"totalTime\":%lf,\"selfTime\":%lf,\"totalSamplesCount\":%lf,\"selfSamplesCount\":%lf,\"callUID\":%u,\"visible\":true,\"numberOfCalls\":0,\"childrenCount\":%d,\"children\":[", functionName, scriptName, scriptName, node->GetLineNumber(), node->GetTotalTime(), node->GetSelfTime(), node->GetTotalSamplesCount(), node->GetSelfSamplesCount(), node->GetCallUid(), node->GetChildrenCount());

		snapshot = String::Concat(snapshot, String::New(leading));

		for (int ii = 0, len = node->GetChildrenCount(); ii < len; ++ii) {
			if (ii > 0) {
				snapshot = String::Concat(snapshot, String::New(","));
			}
			parseTree(snapshot, node->GetChild(ii));
		}

		snapshot = String::Concat(snapshot, String::New("]}"));
	}

	// Added this one -cat
	static Handle<Array> splitData(Handle<String> &snapshot) {
		const int segSize = 999;

		// Get ASCII C string
		String::Utf8Value snapshotUtf8(snapshot);
		const char *snapshotCStr = *snapshotUtf8 ? *snapshotUtf8 : "<string conversion failed>";

		// Calculate ASCII string length
		int len = (int)strlen(snapshotCStr);

		// Calculate approximate number of array elements
		int segCount = (len + segSize - 1) / segSize;
		Handle<Array> result = Array::New(segCount);

		// For each element,
		int index = 0;
		for (int ii = 0; ii < len; ii += segSize) {
			// Calculate segment size
			int sz = segSize;
			if (ii + segSize > len) {
				sz = len - ii;
			}

			// Split string and add it to the array
			result->Set(index++, String::New(snapshotCStr + ii, sz));
		}

		return result;
	}

    void CpuProfiler::Initialize(Handle<ObjectTemplate> target) {
        HandleScope scope;

        cpu_profiler_template_ = Persistent<ObjectTemplate>::New(ObjectTemplate::New());

        NODE_SET_PROTOTYPE_METHOD2(cpu_profiler_template_, "getProfilesCount", CpuProfiler::GetProfilesCount);
        NODE_SET_PROTOTYPE_METHOD2(cpu_profiler_template_, "getProfile", CpuProfiler::GetProfile);
        NODE_SET_PROTOTYPE_METHOD2(cpu_profiler_template_, "findProfile", CpuProfiler::FindProfile);
        NODE_SET_PROTOTYPE_METHOD2(cpu_profiler_template_, "startProfiling", CpuProfiler::StartProfiling);
        NODE_SET_PROTOTYPE_METHOD2(cpu_profiler_template_, "stopProfiling", CpuProfiler::StopProfiling);
        NODE_SET_PROTOTYPE_METHOD2(cpu_profiler_template_, "deleteAllProfiles", CpuProfiler::DeleteAllProfiles);

        NODE_SET_PROTOTYPE_METHOD2(cpu_profiler_template_, "getProfileHeaders", CpuProfiler::GetProfileHeaders);

        target->Set(String::NewSymbol("cpuProfiler"), cpu_profiler_template_);
    }

    CpuProfiler::CpuProfiler() {}
    CpuProfiler::~CpuProfiler() {}

    Handle<Value> CpuProfiler::GetProfilesCount(const Arguments& args) {
		ENTER_ISOLATE

        return scope.Close(Integer::New(v8::CpuProfiler::GetProfilesCount()));
    }

    Handle<Value> CpuProfiler::GetProfile(const Arguments& args) {
		ENTER_ISOLATE

        if (args.Length() < 1) {
            return ThrowException(Exception::Error(String::New("No index specified")));
        } else if (!args[0]->IsInt32()) {
            return ThrowException(Exception::TypeError(String::New("Argument must be an integer")));
        }
        int32_t index = args[0]->Int32Value();

		if (index >= v8::CpuProfiler::GetProfilesCount()) {
			return Undefined();
		}

        const CpuProfile* profile = v8::CpuProfiler::GetProfile(index);
		if (profile) {
			Local<String> snapshot = String::New("");
			parseTree(snapshot, profile->GetTopDownRoot());

			Handle<Array> result = splitData(snapshot);
			result->Set(String::New("title"), profile->GetTitle());
			result->Set(String::New("uid"), Number::New(profile->GetUid()));

			return scope.Close(result);
		} else {
			return Undefined();
		}
    }

    Handle<Value> CpuProfiler::FindProfile(const Arguments& args) {
		ENTER_ISOLATE

        if (args.Length() < 1) {
            return ThrowException(Exception::Error(String::New("No index specified")));
        } else if (!args[0]->IsInt32()) {
            return ThrowException(Exception::TypeError(String::New("Argument must be an integer")));
        }
        uint32_t uid = args[0]->Uint32Value();
        const CpuProfile* profile = v8::CpuProfiler::FindProfile(uid);
		if (profile) {
			Local<String> snapshot = String::New("");
			parseTree(snapshot, profile->GetTopDownRoot());

			Handle<Array> result = splitData(snapshot);
			result->Set(String::New("title"), profile->GetTitle());
			result->Set(String::New("uid"), Number::New(profile->GetUid()));

			return scope.Close(result);
		} else {
			return Undefined();
		}
    }

    Handle<Value> CpuProfiler::StartProfiling(const Arguments& args) {
		ENTER_ISOLATE

        Local<String> title = args.Length() > 0 ? args[0]->ToString() : String::New("");
        v8::CpuProfiler::StartProfiling(title);
        return Undefined();
    }

    Handle<Value> CpuProfiler::StopProfiling(const Arguments& args) {
		ENTER_ISOLATE

        Local<String> title = args.Length() > 0 ? args[0]->ToString() : String::New("");
        const CpuProfile* profile = v8::CpuProfiler::StopProfiling(title);
		if (profile) {
			Local<String> snapshot = String::New("");
			parseTree(snapshot, profile->GetTopDownRoot());

			Handle<Array> result = splitData(snapshot);
			result->Set(String::New("title"), profile->GetTitle());
			result->Set(String::New("uid"), Number::New(profile->GetUid()));
    	    return scope.Close(result);
		} else {
			return Undefined();
		}
    }

	Handle<Value> CpuProfiler::GetProfileHeaders(const Arguments& args) {
		ENTER_ISOLATE

		char desc[512];

		int profilesCount = v8::CpuProfiler::GetProfilesCount();

		Handle<Array> result = Array::New(profilesCount);

		for (int ii = 0; ii < profilesCount; ++ii) {
			const CpuProfile *profile = v8::CpuProfiler::GetProfile(ii);
			unsigned int uid = profile->GetUid();

			String::Utf8Value title8(profile->GetTitle());
			const char *title = *title8 ? *title8 : "<string conversion failed>";

			snprintf(desc, sizeof(desc), "{\"title\":\"%s\",\"uid\":%u,\"typeId\":\"CPU\"}", title, uid);

			result->Set(ii, String::New(desc));
		}

		return result;
	}

    Handle<Value> CpuProfiler::DeleteAllProfiles(const Arguments& args) {
		ENTER_ISOLATE

        v8::CpuProfiler::DeleteAllProfiles();
        return Undefined();
    }
} //namespace nodex
