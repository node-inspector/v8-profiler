#include "snapshot.h"
#include "snapshot_diff.h"
#include "graph_node.h"

using namespace v8;

namespace nodex {
  
Persistent<ObjectTemplate> Snapshot::snapshot_template_;

void Snapshot::Initialize() {
  snapshot_template_ = Persistent<ObjectTemplate>::New(ObjectTemplate::New());
  snapshot_template_->SetInternalFieldCount(1);
  snapshot_template_->SetAccessor(String::New("title"), Snapshot::GetTitle);
  snapshot_template_->SetAccessor(String::New("uid"), Snapshot::GetUid);
  snapshot_template_->SetAccessor(String::New("root"), Snapshot::GetRoot);
  snapshot_template_->SetAccessor(String::New("type"), Snapshot::GetType);
  snapshot_template_->Set(String::New("compareWith"), FunctionTemplate::New(Snapshot::CompareWith));
}

Handle<Value> Snapshot::GetUid(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  uint32_t uid = static_cast<HeapSnapshot*>(ptr)->GetUid();
  return scope.Close(Integer::NewFromUnsigned(uid));
}


Handle<Value> Snapshot::GetTitle(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  Handle<String> title = static_cast<HeapSnapshot*>(ptr)->GetTitle();
  return scope.Close(title);
}

Handle<Value> Snapshot::GetRoot(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  const HeapGraphNode* node = static_cast<HeapSnapshot*>(ptr)->GetRoot();
  return scope.Close(GraphNode::New(node));
}

Handle<Value> Snapshot::GetType(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  HeapSnapshot::Type type = static_cast<HeapSnapshot*>(ptr)->GetType();
  Local<String> t;
  switch(type) {
    case HeapSnapshot::kFull :
    t = String::New("Full");
    break;
    case HeapSnapshot::kAggregated :
    t = String::New("Aggregated");
    break;
    default:
    t = String::New("Unknown");
    break;
  }
  return scope.Close(t);
}

Handle<Value> Snapshot::CompareWith(const Arguments& args) {
  HandleScope scope;
  if (args.Length() < 1) {
    return ThrowException(Exception::Error(String::New("No index specified")));
  }
  Handle<Object> other = args[0]->ToObject();
  Handle<Object> self = args.This();
  
  void* ptr = self->GetPointerFromInternalField(0);
  
  void* optr = other->GetPointerFromInternalField(0);
  
  const HeapSnapshotsDiff* diff = static_cast<HeapSnapshot*>(ptr)->CompareWith(static_cast<HeapSnapshot*>(optr));
  return scope.Close(SnapshotDiff::New(diff));
}

Handle<Value> Snapshot::New(const HeapSnapshot* snapshot) {
  HandleScope scope;
  
  if (snapshot_template_.IsEmpty()) {
    Snapshot::Initialize();
  }
  
  if(!snapshot) {
    return Undefined();
  }
  else {
    Local<Object> obj = snapshot_template_->NewInstance();
    obj->SetPointerInInternalField(0, const_cast<HeapSnapshot*>(snapshot));
    return scope.Close(obj);
  }
}
}