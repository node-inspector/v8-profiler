#include "heap_profiler.h"
#include "snapshot.h"

namespace nodex {
    Persistent<ObjectTemplate> HeapProfiler::heap_profiler_template_;

    class ActivityControlAdapter : public ActivityControl {
        public:
            ActivityControlAdapter(Handle<Value> progress)
                :   reportProgress(Handle<Function>::Cast(progress)),
                    abort(Local<Value>::New(Boolean::New(false))) {}

            ControlOption ReportProgressValue(int done, int total) {
                HandleScope scope;

                Local<Value> argv[2] = {
                    Integer::New(done),
                    Integer::New(total)
                };

                TryCatch try_catch;

                abort = reportProgress->Call(Context::GetCurrent()->Global(), 2, argv);

                if (try_catch.HasCaught()) {
                    FatalException(try_catch);
                    return kAbort;
                }

                fprintf(stderr, "here!\n");

                if (abort.IsEmpty() || !abort->IsBoolean()) {
                    return kContinue;
                }

                return abort->IsTrue() ? kAbort : kContinue;
            }

        private:
            Handle<Function> reportProgress; 
            Local<Value> abort;
    };

    void HeapProfiler::Initialize(Handle<Object> target) {
        HandleScope scope;

        heap_profiler_template_ = Persistent<ObjectTemplate>::New(ObjectTemplate::New());
        heap_profiler_template_->SetInternalFieldCount(1);

        Local<Object> heapProfilerObj = heap_profiler_template_->NewInstance();

        NODE_SET_METHOD(heapProfilerObj, "takeSnapshot", HeapProfiler::TakeSnapshot);
        NODE_SET_METHOD(heapProfilerObj, "getSnapshot", HeapProfiler::GetSnapshot);
        NODE_SET_METHOD(heapProfilerObj, "findSnapshot", HeapProfiler::FindSnapshot);
        NODE_SET_METHOD(heapProfilerObj, "getSnapshotsCount", HeapProfiler::GetSnapshotsCount);
        NODE_SET_METHOD(heapProfilerObj, "deleteAllSnapshots", HeapProfiler::DeleteAllSnapshots);

        target->Set(String::NewSymbol("heapProfiler"), heapProfilerObj);
    }

    HeapProfiler::HeapProfiler() {}
    HeapProfiler::~HeapProfiler() {}

    Handle<Value> HeapProfiler::GetSnapshotsCount(const Arguments& args) {
        HandleScope scope;
        return scope.Close(Integer::New(v8::HeapProfiler::GetSnapshotsCount()));
    }

    Handle<Value> HeapProfiler::GetSnapshot(const Arguments& args) {
        HandleScope scope;
        if (args.Length() < 1) {
            return ThrowException(Exception::Error(String::New("No index specified")));
        } else if (!args[0]->IsInt32()) {
            return ThrowException(Exception::TypeError(String::New("Argument must be an integer")));
        }
        int32_t index = args[0]->Int32Value();
        const v8::HeapSnapshot* snapshot = v8::HeapProfiler::GetSnapshot(index);

        return scope.Close(Snapshot::New(snapshot));
    }

    Handle<Value> HeapProfiler::FindSnapshot(const Arguments& args) {
        HandleScope scope;
        if (args.Length() < 1) {
            return ThrowException(Exception::Error(String::New("No uid specified")));
        }

        uint32_t uid = args[0]->Uint32Value();
        const v8::HeapSnapshot* snapshot = v8::HeapProfiler::FindSnapshot(uid);

        return scope.Close(Snapshot::New(snapshot));
    }

    Handle<Value> HeapProfiler::TakeSnapshot(const Arguments& args) {
        HandleScope scope;
        Local<String> title = String::New("");
        uint32_t len = args.Length();

        ActivityControlAdapter *control = NULL;

        if (len == 1) {
            if (args[0]->IsString()) {
                title = args[0]->ToString();
            } else if (args[0]->IsFunction()) {
                //control = new ActivityControlAdapter(args[0]);
            }
        }

        if (len == 2) {
            if (args[0]->IsString()) {
                title = args[0]->ToString();
            }

            if (args[1]->IsFunction()) {
                //control = new ActivityControlAdapter(args[1]);
            }
        }

        const v8::HeapSnapshot* snapshot = v8::HeapProfiler::TakeSnapshot(title, HeapSnapshot::kFull, control);

        return scope.Close(Snapshot::New(snapshot));
    }

    Handle<Value> HeapProfiler::DeleteAllSnapshots(const Arguments& args) {
        HandleScope scope;
        v8::HeapProfiler::DeleteAllSnapshots();
        return Undefined();
    }
} //namespace nodex
