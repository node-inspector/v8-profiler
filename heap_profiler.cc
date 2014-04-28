#include "heap_profiler.h"
#include "snapshot.h"

namespace nodex {
    Persistent<ObjectTemplate> HeapProfiler::heap_profiler_template_;

    class ActivityControlAdapter : public ActivityControl {
        public:
            ActivityControlAdapter(Handle<Value> progress)
                :   reportProgress(Handle<Function>::Cast(progress)),
                    abort(NanNewLocal<Value>(Boolean::New(false))) {}

            ControlOption ReportProgressValue(int done, int total) {
                NanScope();

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
        NanScope();

        Local<ObjectTemplate> tpl = NanNewLocal<ObjectTemplate>(ObjectTemplate::New());
        NanAssignPersistent(ObjectTemplate, heap_profiler_template_, tpl);
        tpl->SetInternalFieldCount(1);

        Local<Object> heapProfilerObj = tpl->NewInstance();

        NODE_SET_METHOD(heapProfilerObj, "takeSnapshot", HeapProfiler::TakeSnapshot);
        NODE_SET_METHOD(heapProfilerObj, "getSnapshot", HeapProfiler::GetSnapshot);
        NODE_SET_METHOD(heapProfilerObj, "findSnapshot", HeapProfiler::FindSnapshot);
        NODE_SET_METHOD(heapProfilerObj, "getSnapshotsCount", HeapProfiler::GetSnapshotsCount);
        NODE_SET_METHOD(heapProfilerObj, "deleteAllSnapshots", HeapProfiler::DeleteAllSnapshots);

        target->Set(String::NewSymbol("heapProfiler"), heapProfilerObj);
    }

    HeapProfiler::HeapProfiler() {}
    HeapProfiler::~HeapProfiler() {}

    NAN_METHOD(HeapProfiler::GetSnapshotsCount) {
        NanScope();
#if (NODE_MODULE_VERSION > 0x000B)
        NanReturnValue(Integer::New(nan_isolate->GetHeapProfiler()->GetSnapshotCount()));
#else
        NanReturnValue(Integer::New(v8::HeapProfiler::GetSnapshotsCount()));
#endif
    }

    NAN_METHOD(HeapProfiler::GetSnapshot) {
        NanScope();
        if (args.Length() < 1) {
            return NanThrowError("No index specified");
        } else if (!args[0]->IsInt32()) {
            return NanThrowError("Argument must be an integer");
        }
        int32_t index = args[0]->Int32Value();
#if (NODE_MODULE_VERSION > 0x000B)
        const v8::HeapSnapshot* snapshot = nan_isolate->GetHeapProfiler()->GetHeapSnapshot(index);
#else
        const v8::HeapSnapshot* snapshot = v8::HeapProfiler::GetSnapshot(index);
#endif
        NanReturnValue(Snapshot::New(snapshot));
    }

    NAN_METHOD(HeapProfiler::FindSnapshot) {
        NanScope();
        if (args.Length() < 1) {
            return NanThrowError("No index specified");
        } else if (!args[0]->IsInt32()) {
            return NanThrowError("Argument must be an integer");
        }
        uint32_t uid = args[0]->Uint32Value();
        const v8::HeapSnapshot* snapshot;

#if (NODE_MODULE_VERSION > 0x000B)
        bool notFound = true;
        int count = nan_isolate->GetHeapProfiler()->GetSnapshotCount();
        for (int32_t index = 1; index < count; index++) {
            snapshot = nan_isolate->GetHeapProfiler()->GetHeapSnapshot(index);
            if (snapshot->GetUid() == uid) {
                notFound = false;
                break;
            }
        }
        if (notFound) {
            NanReturnNull();
        }
#else
        snapshot = v8::HeapProfiler::FindSnapshot(uid);
#endif
        NanReturnValue(Snapshot::New(snapshot));
    }

    NAN_METHOD(HeapProfiler::TakeSnapshot) {
        NanScope();
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

#if (NODE_MODULE_VERSION > 0x000B)
        const v8::HeapSnapshot* snapshot = nan_isolate->GetHeapProfiler()->TakeHeapSnapshot(title, control);
#else
        const v8::HeapSnapshot* snapshot = v8::HeapProfiler::TakeSnapshot(title, HeapSnapshot::kFull, control);
#endif

        NanReturnValue(Snapshot::New(snapshot));
    }

    NAN_METHOD(HeapProfiler::DeleteAllSnapshots) {
        NanScope();
#if (NODE_MODULE_VERSION > 0x000B)
        nan_isolate->GetHeapProfiler()->DeleteAllHeapSnapshots();
#else
        v8::HeapProfiler::DeleteAllSnapshots();
#endif
        NanReturnUndefined();
    }
} //namespace nodex
