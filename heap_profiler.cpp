#include "heap_profiler.h"
#include "profiler.h"

namespace nodex {
    Persistent<ObjectTemplate> HeapProfiler::heap_profiler_template_;

	// Adapted from WebCore/bindings/v8/ScriptHeapSnapshot.cpp
	class JSONOutputStream : public v8::OutputStream {
		Handle<Array> &_result;
		int _index;

	public:
		JSONOutputStream(Handle<Array> &result)
			: _result(result) {
			_index = 0;
		}
		void EndOfStream() {
			// Done!
		}
		int GetChunkSize() {
			return 16384;
		}
		OutputEncoding GetOutputEncoding() {
			return kAscii;
		}
		WriteResult WriteAsciiChunk(char *data, int size) {
			_result->Set(_index++, String::New(data, size));
			return kContinue;
		}
		WriteResult WriteHeapStatsChunk(HeapStatsUpdate *data, int count) {
			// Ignored for now
			return kContinue;
		}
	};

    void HeapProfiler::Initialize(Handle<ObjectTemplate> target) {
        HandleScope scope;

        heap_profiler_template_ = Persistent<ObjectTemplate>::New(ObjectTemplate::New());
        NODE_SET_PROTOTYPE_METHOD2(heap_profiler_template_, "takeSnapshot", HeapProfiler::TakeSnapshot);
        NODE_SET_PROTOTYPE_METHOD2(heap_profiler_template_, "getSnapshot", HeapProfiler::GetSnapshot);
        NODE_SET_PROTOTYPE_METHOD2(heap_profiler_template_, "findSnapshot", HeapProfiler::FindSnapshot);
        NODE_SET_PROTOTYPE_METHOD2(heap_profiler_template_, "getSnapshotsCount", HeapProfiler::GetSnapshotsCount);
        NODE_SET_PROTOTYPE_METHOD2(heap_profiler_template_, "deleteAllSnapshots", HeapProfiler::DeleteAllSnapshots);

        target->Set(String::NewSymbol("heapProfiler"), heap_profiler_template_);
    }

    HeapProfiler::HeapProfiler() {}
    HeapProfiler::~HeapProfiler() {}

    Handle<Value> HeapProfiler::GetSnapshotsCount(const Arguments& args) {
		ENTER_ISOLATE

        return scope.Close(Integer::New(v8::HeapProfiler::GetSnapshotsCount()));
    }

    Handle<Value> HeapProfiler::GetSnapshot(const Arguments& args) {
		ENTER_ISOLATE

        if (args.Length() < 1) {
            return ThrowException(Exception::Error(String::New("No index specified")));
        } else if (!args[0]->IsInt32()) {
            return ThrowException(Exception::TypeError(String::New("Argument must be an integer")));
        }
        int32_t index = args[0]->Int32Value();
        const v8::HeapSnapshot* snapshot = v8::HeapProfiler::GetSnapshot(index);
		if (snapshot) {
			Handle<Array> result = Array::New(0);
			JSONOutputStream stream(result);

			snapshot->Serialize(&stream, v8::HeapSnapshot::kJSON);

			return scope.Close(result);
		} else {
			return Undefined();
		}
    }

    Handle<Value> HeapProfiler::FindSnapshot(const Arguments& args) {
		ENTER_ISOLATE

		if (args.Length() < 1) {
            return ThrowException(Exception::Error(String::New("No uid specified")));
        }

        uint32_t uid = args[0]->Uint32Value();
        const v8::HeapSnapshot* snapshot = v8::HeapProfiler::FindSnapshot(uid);
		if (snapshot) {
			Handle<Array> result = Array::New(0);
			JSONOutputStream stream(result);

			snapshot->Serialize(&stream, v8::HeapSnapshot::kJSON);

			return scope.Close(result);
		} else {
			return Undefined();
		}
    }

    Handle<Value> HeapProfiler::TakeSnapshot(const Arguments& args) {
		ENTER_ISOLATE

        Local<String> title = String::New("");

        if (args.Length() >= 1) {
            if (args[0]->IsString()) {
                title = args[0]->ToString();
            }
        }

        const v8::HeapSnapshot* snapshot = v8::HeapProfiler::TakeSnapshot(title, HeapSnapshot::kFull, NULL);
		if (snapshot) {
			Handle<Array> result = Array::New(0);
			JSONOutputStream stream(result);

			snapshot->Serialize(&stream, v8::HeapSnapshot::kJSON);

			return scope.Close(result);
		} else {
			return Undefined();
		}
    }

    Handle<Value> HeapProfiler::DeleteAllSnapshots(const Arguments& args) {
		ENTER_ISOLATE

        v8::HeapProfiler::DeleteAllSnapshots();
        return Undefined();
    }
} //namespace nodex
