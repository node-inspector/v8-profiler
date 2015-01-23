#include "heap_snapshot.h"
#include "heap_output_stream.h"
#include "heap_graph_node.h"

namespace nodex {
  using v8::Array;
  using v8::Handle;
  using v8::HeapSnapshot;
  using v8::HeapGraphNode;
  using v8::HeapGraphEdge;
  using v8::Integer;
  using v8::Local;
  using v8::Object;
  using v8::ObjectTemplate;
  using v8::FunctionTemplate;
  using v8::Persistent;
  using v8::SnapshotObjectId;
  using v8::String;
  using v8::Function;
  using v8::Value;
  

  Persistent<ObjectTemplate> Snapshot::snapshot_template_;
  Persistent<Array> Snapshot::snapshots;

  void Snapshot::Initialize () {
    NanScope();
    
    Local<FunctionTemplate> f = NanNew<FunctionTemplate>();
    Local<ObjectTemplate> o = f->InstanceTemplate();
    o->SetInternalFieldCount(1);
    o->SetAccessor(NanNew<String>("root"), Snapshot::GetRoot);
    NODE_SET_METHOD(o, "getNode", Snapshot::GetNode);
    NODE_SET_METHOD(o, "getNodeById", Snapshot::GetNodeById);
    NODE_SET_METHOD(o, "delete", Snapshot::Delete);
    NODE_SET_METHOD(o, "serialize", Snapshot::Serialize);
    NanAssignPersistent(snapshot_template_, o);
  }
  
  NAN_GETTER(Snapshot::GetRoot) {
    NanScope();
    
    Handle<Object> _root;
    if (args.This()->Has(NanNew<String>("_root"))) { 
      NanReturnValue(args.This()->GetHiddenValue(NanNew<String>("_root")));
    } else {
      void* ptr = NanGetInternalFieldPointer(args.This(), 0);
      Handle<Value> _root = GraphNode::New(static_cast<HeapSnapshot*>(ptr)->GetRoot());
      args.This()->SetHiddenValue(NanNew<String>("_root"), _root);
      NanReturnValue(_root);
    }
  }
  
  NAN_METHOD(Snapshot::GetNode) {
    NanScope();
    
    if (!args.Length()) {
      return NanThrowError("No index specified");
    } else if (!args[0]->IsInt32()) {
      return NanThrowTypeError("Argument must be an integer");
    }
    
    int32_t index = args[0]->Int32Value();
    void* ptr = NanGetInternalFieldPointer(args.This(), 0);
    NanReturnValue(GraphNode::New(static_cast<HeapSnapshot*>(ptr)->GetNode(index)));
  }
  
  NAN_METHOD(Snapshot::GetNodeById) {
    NanScope();
    
    if (!args.Length()) {
      return NanThrowError("No id specified");
    } else if (!args[0]->IsInt32()) {
      return NanThrowTypeError("Argument must be an integer");
    }
    
    SnapshotObjectId id = args[0]->Int32Value();
    void* ptr = NanGetInternalFieldPointer(args.This(), 0);
    NanReturnValue(GraphNode::New(static_cast<HeapSnapshot*>(ptr)->GetNodeById(id)));
  }
  
  NAN_METHOD(Snapshot::Serialize) {
    NanScope();
    
    void* ptr = NanGetInternalFieldPointer(args.This(), 0);
    if (args.Length() < 2) {
      return NanThrowError("Invalid number of arguments");
    } else if (!args[0]->IsFunction() || !args[1]->IsFunction()) {
      return NanThrowTypeError("Arguments must be a functions");
    }

    Handle<Function> iterator = Handle<Function>::Cast(args[0]);
    Handle<Function> callback = Handle<Function>::Cast(args[1]);
  
    OutputStreamAdapter *stream = new OutputStreamAdapter(iterator, callback);
    static_cast<HeapSnapshot*>(ptr)->Serialize(stream, HeapSnapshot::kJSON);
    
    NanReturnUndefined();
  }  

  NAN_METHOD(Snapshot::Delete) {
    NanScope();

    void* ptr = NanGetInternalFieldPointer(args.Holder(), 0);
    Local<Array> snapshots = NanNew<Array>(Snapshot::snapshots);
    
    uint32_t count = snapshots->Length();
    for (uint32_t index = 0; index < count; index++) {
      if (snapshots->Get(index) == args.This()) {
        Local<Value> argv[2] = {
          NanNew<Integer>(index),
          NanNew<Integer>(1)
        };
        Handle<Function>::Cast(snapshots->Get(NanNew<String>("splice")))->Call(snapshots, 2, argv);
        break;
      }
    }
    static_cast<HeapSnapshot*>(ptr)->Delete();

    NanReturnUndefined();
  }
  
  Handle<Value> Snapshot::New(const HeapSnapshot* node) {
    NanEscapableScope();

    if (snapshot_template_.IsEmpty()) {
      Snapshot::Initialize();
    }

    Local<Object> snapshot = NanNew(snapshot_template_)->NewInstance();
    NanSetInternalFieldPointer(snapshot, 0, const_cast<HeapSnapshot*>(node));

    Local<Value> HEAP = NanNew<String>("HEAP");
    uint16_t _uid = node->GetUid();
    Local<Value> uid = NanNew<Integer>(_uid);
    Handle<String> title = node->GetTitle();
    if (!title->Length()) {
      char _title[32];
      sprintf(_title, "Snapshot %i", _uid);
      title = NanNew<String>(_title);
    }
    Local<Integer> nodesCount = NanNew<Integer>(node->GetNodesCount());
    Local<Integer> objectId = NanNew<Integer>(node->GetMaxSnapshotJSObjectId());

    snapshot->Set(NanNew<String>("typeId"), HEAP);
    snapshot->Set(NanNew<String>("title"), title);
    snapshot->Set(NanNew<String>("uid"), uid);
    snapshot->Set(NanNew<String>("nodesCount"), nodesCount);
    snapshot->Set(NanNew<String>("maxSnapshotJSObjectId"), objectId);

    Local<Array> snapshots = NanNew<Array>(Snapshot::snapshots);
    snapshots->Set(snapshots->Length(), snapshot);

    return NanEscapeScope(snapshot);
  }
}
