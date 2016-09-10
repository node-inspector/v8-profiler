#include "heap_snapshot.h"
#include "heap_output_stream.h"
#include "heap_graph_node.h"

namespace {
#if (NODE_MODULE_VERSION > 48)
  // The two methods below were copied from
  // https://github.com/electron/electron?branch=master&filepath=atom/common/api/atom_api_v8_util.cc
  // Copyright (c) 2013 GitHub, Inc.
  // Use of this source code is governed by the MIT license.

  v8::Local<v8::Value> GetHiddenValue(v8::Isolate* isolate,
                                      v8::Local<v8::Object> object,
                                      v8::Local<v8::String> key) {
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Local<v8::Private> privateKey = v8::Private::ForApi(isolate, key);
    v8::Local<v8::Value> value;
    v8::Maybe<bool> result = object->HasPrivate(context, privateKey);
    if (!(result.IsJust() && result.FromJust()))
      return v8::Local<v8::Value>();
    if (object->GetPrivate(context, privateKey).ToLocal(&value))
      return value;
    return v8::Local<v8::Value>();
  }

  void SetHiddenValue(v8::Isolate* isolate,
                      v8::Local<v8::Object> object,
                      v8::Local<v8::String> key,
                      v8::Local<v8::Value> value) {
    if (value.IsEmpty())
      return;
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Local<v8::Private> privateKey = v8::Private::ForApi(isolate, key);
    object->SetPrivate(context, privateKey, value);
  }
#else
  v8::Local<v8::Value> GetHiddenValue(v8::Isolate* isolate,
                                      v8::Local<v8::Object> object,
                                      v8::Local<v8::String> key) {
    return object->GetHiddenValue(key);
  }

  void SetHiddenValue(v8::Isolate* isolate,
                      v8::Local<v8::Object> object,
                      v8::Local<v8::String> key,
                      v8::Local<v8::Value> value) {
    object->SetHiddenValue(key, value);
  }
#endif
}

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
  using v8::SnapshotObjectId;
  using v8::String;
  using v8::Function;
  using v8::Value;


  Nan::Persistent<ObjectTemplate> Snapshot::snapshot_template_;
  Nan::Persistent<Object> Snapshot::snapshots;

  NAN_METHOD(Snapshot_EmptyMethod) {
  }

  void Snapshot::Initialize () {
    Nan::HandleScope scope;

    Local<FunctionTemplate> f = Nan::New<FunctionTemplate>(Snapshot_EmptyMethod);
    Local<ObjectTemplate> o = f->InstanceTemplate();
    o->SetInternalFieldCount(1);
    Nan::SetAccessor(o, Nan::New<String>("root").ToLocalChecked(), Snapshot::GetRoot);
    Nan::SetMethod(o, "getNode", Snapshot::GetNode);
    Nan::SetMethod(o, "getNodeById", Snapshot::GetNodeById);
    Nan::SetMethod(o, "delete", Snapshot::Delete);
    Nan::SetMethod(o, "serialize", Snapshot::Serialize);
    snapshot_template_.Reset(o);
  }

  NAN_GETTER(Snapshot::GetRoot) {
    Local<Object> _root;
    Local<String> __root = Nan::New<String>("_root").ToLocalChecked();
    if (info.This()->Has(__root)) {
      info.GetReturnValue().Set(GetHiddenValue(info.GetIsolate(), info.This(), __root));
    } else {
      void* ptr = Nan::GetInternalFieldPointer(info.This(), 0);
      Local<Value> _root = GraphNode::New(static_cast<HeapSnapshot*>(ptr)->GetRoot());
      SetHiddenValue(info.GetIsolate(), info.This(), __root, _root);
      info.GetReturnValue().Set(_root);
    }
  }

  NAN_METHOD(Snapshot::GetNode) {
    if (!info.Length()) {
      return Nan::ThrowError("No index specified");
    } else if (!info[0]->IsInt32()) {
      return Nan::ThrowTypeError("Argument must be an integer");
    }

    int32_t index = info[0]->Int32Value();
    void* ptr = Nan::GetInternalFieldPointer(info.This(), 0);
    info.GetReturnValue().Set(GraphNode::New(static_cast<HeapSnapshot*>(ptr)->GetNode(index)));
  }

  NAN_METHOD(Snapshot::GetNodeById) {
    if (!info.Length()) {
      return Nan::ThrowError("No id specified");
    } else if (!info[0]->IsInt32()) {
      return Nan::ThrowTypeError("Argument must be an integer");
    }

    SnapshotObjectId id = info[0]->Int32Value();
    void* ptr = Nan::GetInternalFieldPointer(info.This(), 0);
    info.GetReturnValue().Set(GraphNode::New(static_cast<HeapSnapshot*>(ptr)->GetNodeById(id)));
  }

  NAN_METHOD(Snapshot::Serialize) {
    void* ptr = Nan::GetInternalFieldPointer(info.This(), 0);
    if (info.Length() < 2) {
      return Nan::ThrowError("Invalid number of arguments");
    } else if (!info[0]->IsFunction() || !info[1]->IsFunction()) {
      return Nan::ThrowTypeError("Arguments must be a functions");
    }

    Local<Function> iterator = Local<Function>::Cast(info[0]);
    Local<Function> callback = Local<Function>::Cast(info[1]);

    OutputStreamAdapter *stream = new OutputStreamAdapter(iterator, callback);
    static_cast<HeapSnapshot*>(ptr)->Serialize(stream, HeapSnapshot::kJSON);
  }

  NAN_METHOD(Snapshot::Delete) {
    void* ptr = Nan::GetInternalFieldPointer(info.Holder(), 0);

    Local<Object> snapshots = Nan::New<Object>(Snapshot::snapshots);

    Local<String> __uid = Nan::New<String>("uid").ToLocalChecked();
    Local<Integer> _uid = Nan::To<Integer>(Nan::Get(info.Holder(), __uid).ToLocalChecked()).ToLocalChecked();
    Nan::Delete(snapshots, _uid->Value());

    static_cast<HeapSnapshot*>(ptr)->Delete();
    info.GetReturnValue().Set(snapshots);
  }

  Local<Value> Snapshot::New(const HeapSnapshot* node) {
    Nan::EscapableHandleScope scope;

    if (snapshot_template_.IsEmpty()) {
      Snapshot::Initialize();
    }

    Local<Object> snapshot = Nan::New(snapshot_template_)->NewInstance();
    Nan::SetInternalFieldPointer(snapshot, 0, const_cast<HeapSnapshot*>(node));

    Local<Value> HEAP = Nan::New<String>("HEAP").ToLocalChecked();

    // starting with iojs 3 GetUid() and GetTitle() APIs were removed
    uint32_t _uid = node->GetMaxSnapshotJSObjectId();

    char _title[32];
    sprintf(_title, "Snapshot %i", _uid);
    Local<String> title = Nan::New<String>(_title).ToLocalChecked();

    Local<Value> uid = Nan::New<Integer>(_uid);
    Local<Integer> nodesCount = Nan::New<Integer>(node->GetNodesCount());
    Local<Integer> objectId = Nan::New<Integer>(node->GetMaxSnapshotJSObjectId());

    snapshot->Set(Nan::New<String>("typeId").ToLocalChecked(), HEAP);
    snapshot->Set(Nan::New<String>("title").ToLocalChecked(), title);
    snapshot->Set(Nan::New<String>("uid").ToLocalChecked(), uid);
    snapshot->Set(Nan::New<String>("nodesCount").ToLocalChecked(), nodesCount);
    snapshot->Set(Nan::New<String>("maxSnapshotJSObjectId").ToLocalChecked(), objectId);

    Local<Object> snapshots = Nan::New<Object>(Snapshot::snapshots);
    Nan::Set(snapshots, _uid, snapshot);

    return scope.Escape(snapshot);
  }
}
