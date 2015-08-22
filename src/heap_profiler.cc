#include "heap_profiler.h"
#include "heap_snapshot.h"
#include "heap_output_stream.h"

namespace nodex {
  using v8::ActivityControl;
  using v8::Array;
  using v8::Function;
  using v8::Handle;
  using v8::HeapSnapshot;
  using v8::Integer;
  using v8::Local;
  using v8::Object;
  using v8::SnapshotObjectId;
  using v8::String;
  using v8::TryCatch;
  using v8::Value;

  HeapProfiler::HeapProfiler() {}
  HeapProfiler::~HeapProfiler() {}

  class ActivityControlAdapter : public ActivityControl {
    public:
      ActivityControlAdapter(Local<Value> progress)
        : reportProgress(Local<Function>::Cast(progress)),
          abort(Nan::False())
      {}

      ControlOption ReportProgressValue(int done, int total) {
        Local<Value> argv[2] = {
          Nan::New<Integer>(done),
          Nan::New<Integer>(total)
        };

        TryCatch try_catch;
        abort = reportProgress->Call(Nan::GetCurrentContext()->Global(), 2, argv);

        if (try_catch.HasCaught()) {
          Nan::ThrowError(try_catch.Exception());
          return kAbort;
        }

        return abort->IsFalse() ? kAbort : kContinue;
      }

    private:
      Local<Function> reportProgress;
      Local<Value> abort;
  };

  void HeapProfiler::Initialize (Local<Object> target) {
    Nan::HandleScope scope;

    Local<Object> heapProfiler = Nan::New<Object>();
    Local<Array> snapshots = Nan::New<Array>();

    Nan::SetMethod(heapProfiler, "takeSnapshot", HeapProfiler::TakeSnapshot);
    Nan::SetMethod(heapProfiler, "startTrackingHeapObjects", HeapProfiler::StartTrackingHeapObjects);
    Nan::SetMethod(heapProfiler, "stopTrackingHeapObjects", HeapProfiler::StopTrackingHeapObjects);
    Nan::SetMethod(heapProfiler, "getHeapStats", HeapProfiler::GetHeapStats);
    Nan::SetMethod(heapProfiler, "getObjectByHeapObjectId", HeapProfiler::GetObjectByHeapObjectId);
    heapProfiler->Set(Nan::New<String>("snapshots").ToLocalChecked(), snapshots);

    Snapshot::snapshots.Reset(snapshots);
    target->Set(Nan::New<String>("heap").ToLocalChecked(), heapProfiler);
  }

  NAN_METHOD(HeapProfiler::TakeSnapshot) {
    ActivityControlAdapter* control = NULL;
    Local<String> title = Nan::EmptyString();
    if (info.Length()) {
      if (info.Length()>1) {
        if (info[1]->IsFunction()) {
          control = new ActivityControlAdapter(info[0]);
        } else if (!info[1]->IsUndefined()) {
          return Nan::ThrowTypeError("Wrong argument [1] type (wait Function)");
        }
        if (info[0]->IsString()) {
          title = info[0]->ToString();
        } else if (!info[0]->IsUndefined()) {
          return Nan::ThrowTypeError("Wrong argument [0] type (wait String)");
        }
      } else {
        if (info[0]->IsString()) {
          title = info[0]->ToString();
        } else if (info[0]->IsFunction()) {
          control = new ActivityControlAdapter(info[0]);
        } else if (!info[0]->IsUndefined()) {
          return Nan::ThrowTypeError("Wrong argument [0] type (wait String or Function)");
        }
      }
    }


#if (NODE_MODULE_VERSION > 0x002C)
    const HeapSnapshot* snapshot = v8::Isolate::GetCurrent()->GetHeapProfiler()->TakeHeapSnapshot(control);
#elif (NODE_MODULE_VERSION > 0x000B)
    const HeapSnapshot* snapshot = v8::Isolate::GetCurrent()->GetHeapProfiler()->TakeHeapSnapshot(title, control);
#else
    const HeapSnapshot* snapshot = v8::HeapProfiler::TakeSnapshot(title, HeapSnapshot::kFull, control);
#endif

    info.GetReturnValue().Set(Snapshot::New(snapshot));
  }

  NAN_METHOD(HeapProfiler::StartTrackingHeapObjects) {
#if (NODE_MODULE_VERSION > 0x000B)
    v8::Isolate::GetCurrent()->GetHeapProfiler()->StartTrackingHeapObjects();
#else
    v8::HeapProfiler::StartHeapObjectsTracking();
#endif

    return;
  }

  NAN_METHOD(HeapProfiler::GetObjectByHeapObjectId) {
    if (info.Length() < 1) {
      return Nan::ThrowError("Invalid number of arguments");
    } else if (!info[0]->IsNumber()) {
      return Nan::ThrowTypeError("Arguments must be a number");
    }

    SnapshotObjectId id = info[0]->Uint32Value();
    Local<Value> object;
#if (NODE_MODULE_VERSION > 0x000B)
    object = v8::Isolate::GetCurrent()->GetHeapProfiler()->FindObjectById(id);
#else
    Local<Array> snapshots = Local<Array>::Cast(info.This()->Get(Nan::New<String>("snapshots").ToLocalChecked()));
    Local<Object> snapshot;
    uint32_t length = snapshots->Length();

    if (length == 0) return;

    for (uint32_t i = 0; i < length; ++i) {
      snapshot = snapshots->Get(i)->ToObject();
      Local<Value> argv[1] = { info[0] };
      if (snapshot->Get(Nan::New<String>("maxSnapshotJSObjectId").ToLocalChecked())->Uint32Value() >= id) {
        Local<Object> graph_node = Function::Cast(*snapshot->Get(Nan::New<String>("getNodeById").ToLocalChecked()))
                                      ->Call(snapshot, 1, argv)->ToObject();
        object = Function::Cast(*graph_node->Get(Nan::New<String>("getHeapValue").ToLocalChecked()))
                  ->Call(graph_node, 0, NULL);
        break;
      }
    }
#endif

    if (object.IsEmpty()) {
      return;
    } else if (object->IsObject()
            || object->IsNumber()
            || object->IsString()
#if (NODE_MODULE_VERSION > 0x000B)
            || object->IsSymbol()
#endif
            || object->IsBoolean()) {
      info.GetReturnValue().Set(object);
    } else {
      info.GetReturnValue().Set(Nan::New<String>("Preview is not available").ToLocalChecked());
    }
  }

  NAN_METHOD(HeapProfiler::StopTrackingHeapObjects) {
#if (NODE_MODULE_VERSION > 0x000B)
    v8::Isolate::GetCurrent()->GetHeapProfiler()->StopTrackingHeapObjects();
#else
    v8::HeapProfiler::StopHeapObjectsTracking();
#endif
  }

  NAN_METHOD(HeapProfiler::GetHeapStats) {
    if (info.Length() < 2) {
      return Nan::ThrowError("Invalid number of arguments");
    } else if (!info[0]->IsFunction() || !info[1]->IsFunction()) {
      return Nan::ThrowTypeError("Argument must be a function");
    }

    Local<Function> iterator = Local<Function>::Cast(info[0]);
    Local<Function> callback = Local<Function>::Cast(info[1]);

    OutputStreamAdapter* stream = new OutputStreamAdapter(iterator, callback);
#if (NODE_MODULE_VERSION > 0x000B)
    SnapshotObjectId ID = v8::Isolate::GetCurrent()->GetHeapProfiler()->GetHeapStats(stream);
#else
    SnapshotObjectId ID = v8::HeapProfiler::PushHeapObjectsStats(stream);
#endif
    info.GetReturnValue().Set(Nan::New<Integer>(ID));
  }
} //namespace nodex
