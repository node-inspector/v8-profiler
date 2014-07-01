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
      ActivityControlAdapter(Handle<Value> progress)
        : reportProgress(Handle<Function>::Cast(progress)),
          abort(NanFalse()) 
      {}

      ControlOption ReportProgressValue(int done, int total) {
        Local<Value> argv[2] = {
          NanNew<Integer>(done),
          NanNew<Integer>(total)
        };

        TryCatch try_catch;
        abort = reportProgress->Call(NanGetCurrentContext()->Global(), 2, argv);

        if (try_catch.HasCaught()) {
          NanThrowError(try_catch.Exception());
          return kAbort;
        }

        return abort->IsFalse() ? kAbort : kContinue;
      }

    private:
      Handle<Function> reportProgress; 
      Handle<Value> abort;
  };
  
  void HeapProfiler::Initialize (Handle<Object> target) {
    NanScope();
    
    Local<Object> heapProfiler = NanNew<Object>();
    Local<Array> snapshots = NanNew<Array>();
    
    NODE_SET_METHOD(heapProfiler, "takeSnapshot", HeapProfiler::TakeSnapshot);    
    NODE_SET_METHOD(heapProfiler, "startTrackingHeapObjects", HeapProfiler::StartTrackingHeapObjects);
    NODE_SET_METHOD(heapProfiler, "stopTrackingHeapObjects", HeapProfiler::StopTrackingHeapObjects);
    NODE_SET_METHOD(heapProfiler, "getHeapStats", HeapProfiler::GetHeapStats);
    NODE_SET_METHOD(heapProfiler, "getObjectByHeapObjectId", HeapProfiler::GetObjectByHeapObjectId);
    heapProfiler->Set(NanNew<String>("snapshots"), snapshots);
    
    NanAssignPersistent(Snapshot::snapshots, snapshots);
    target->Set(NanNew<String>("heap"), heapProfiler);
  }
  
  NAN_METHOD(HeapProfiler::TakeSnapshot) {
    NanScope();
    
    ActivityControlAdapter* control = NULL;
    Local<String> title = NanNew<String>("");
    if (args.Length()) {
      if (args.Length()>1) {
        if (args[1]->IsFunction()) {
          control = new ActivityControlAdapter(args[0]);
        } else if (!args[1]->IsUndefined()) {
          return NanThrowTypeError("Wrong argument [1] type (wait Function)");
        }
        if (args[0]->IsString()) {
          title = args[0]->ToString();
        } else if (!args[0]->IsUndefined()) {
          return NanThrowTypeError("Wrong argument [0] type (wait String)");
        }
      } else {
        if (args[0]->IsString()) {
          title = args[0]->ToString();
        } else if (args[0]->IsFunction()) {
          control = new ActivityControlAdapter(args[0]);
        } else if (!args[0]->IsUndefined()) {
          return NanThrowTypeError("Wrong argument [0] type (wait String or Function)");
        }
      }
    }
    
#if (NODE_MODULE_VERSION > 0x000B)
    const HeapSnapshot* snapshot = v8::Isolate::GetCurrent()->GetHeapProfiler()->TakeHeapSnapshot(title, control);
#else
    const HeapSnapshot* snapshot = v8::HeapProfiler::TakeSnapshot(title, HeapSnapshot::kFull, control);
#endif    
    
    NanReturnValue(Snapshot::New(snapshot));
  }
  
  NAN_METHOD(HeapProfiler::StartTrackingHeapObjects) {
    NanScope();
    
#if (NODE_MODULE_VERSION > 0x000B)
    v8::Isolate::GetCurrent()->GetHeapProfiler()->StartTrackingHeapObjects();
#else
    v8::HeapProfiler::StartHeapObjectsTracking();
#endif
    
    NanReturnUndefined();
  }
  
  NAN_METHOD(HeapProfiler::GetObjectByHeapObjectId) {
    NanScope();
    
    if (args.Length() < 1) {
      return NanThrowError("Invalid number of arguments");
    } else if (!args[0]->IsNumber()) {
      return NanThrowTypeError("Arguments must be a number");
    }
    
    SnapshotObjectId id = args[0]->Uint32Value();
    Local<Value> object;
#if (NODE_MODULE_VERSION > 0x000B)
    object = v8::Isolate::GetCurrent()->GetHeapProfiler()->FindObjectById(id);
#else
    Local<Array> snapshots = Local<Array>::Cast(args.This()->Get(NanNew<String>("snapshots")));
    Local<Object> snapshot;
    uint32_t length = snapshots->Length();
    
    if (length == 0) NanReturnUndefined();
    
    for (uint32_t i = 0; i < length; ++i) {
      snapshot = snapshots->Get(i)->ToObject();
      Local<Value> argv[1] = { args[0] };
      if (snapshot->Get(NanNew<String>("maxSnapshotJSObjectId"))->Uint32Value() >= id) {
        Local<Object> graph_node = Function::Cast(*snapshot->Get(NanNew<String>("getNodeById")))
                                      ->Call(snapshot, 1, argv)->ToObject();
        object = Function::Cast(*graph_node->Get(NanNew<String>("getHeapValue")))
                  ->Call(graph_node, 0, NULL);
        break;
      }
    }
#endif

    if (object.IsEmpty()) {
      NanReturnUndefined();
    } else if (object->IsObject()
            || object->IsNumber()
            || object->IsString()
#if (NODE_MODULE_VERSION > 0x000B)
            || object->IsSymbol()
#endif
            || object->IsBoolean()) {
      NanReturnValue(object);
    } else {
      NanReturnValue(NanNew<String>("Preview is not available"));
    }
  }
  
  NAN_METHOD(HeapProfiler::StopTrackingHeapObjects) {
    NanScope();
    
#if (NODE_MODULE_VERSION > 0x000B)
    v8::Isolate::GetCurrent()->GetHeapProfiler()->StopTrackingHeapObjects();
#else
    v8::HeapProfiler::StopHeapObjectsTracking();
#endif
    
    NanReturnUndefined();
  }
  
  NAN_METHOD(HeapProfiler::GetHeapStats) {
    NanScope();
    if (args.Length() < 2) {
      return NanThrowError("Invalid number of arguments");
    } else if (!args[0]->IsFunction() || !args[1]->IsFunction()) {
      return NanThrowTypeError("Argument must be a function");
    }

    Local<Function> iterator = Local<Function>::Cast(args[0]);
    Local<Function> callback = Local<Function>::Cast(args[1]);
    
    OutputStreamAdapter* stream = new OutputStreamAdapter(iterator, callback);
#if (NODE_MODULE_VERSION > 0x000B)
    SnapshotObjectId ID = v8::Isolate::GetCurrent()->GetHeapProfiler()->GetHeapStats(stream);
#else
    SnapshotObjectId ID = v8::HeapProfiler::PushHeapObjectsStats(stream);
#endif
    NanReturnValue(NanNew<Integer>(ID));
  }
} //namespace nodex
