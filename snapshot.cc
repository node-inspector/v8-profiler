#include "snapshot.h"
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
  snapshot_template_->SetAccessor(String::New("nodesCount"), Snapshot::GetNodesCount);
  snapshot_template_->Set(String::New("getNodeById"), FunctionTemplate::New(Snapshot::GetNodeById));
  snapshot_template_->Set(String::New("getNode"), FunctionTemplate::New(Snapshot::GetNode));
  snapshot_template_->Set(String::New("delete"), FunctionTemplate::New(Snapshot::Delete));
	//TODO: serialize
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
    default:
    t = String::New("Unknown");
    break;
  }
  return scope.Close(t);
}

Handle<Value> Snapshot::GetNodesCount(Local<String> property, const AccessorInfo& info) {
	HandleScope scope;
	Local<Object> self = info.Holder();
	void* ptr = self->GetPointerFromInternalField(0);
	Local<Integer> count = Integer::New(static_cast<HeapSnapshot*>(ptr)->GetNodesCount());
  return scope.Close(count);
}

Handle<Value> Snapshot::GetNodeById(const Arguments& args) {
	HandleScope scope;
	if (args.Length() < 1) {
    return ThrowException(Exception::Error(String::New("No index specified")));
  } else if (!args[0]->IsUint32()) {
    return ThrowException(Exception::Error(String::New("Argument must be integer")));
  }
  uint64_t id = static_cast<uint64_t>(args[0]->Uint32Value());
  Handle<Object> self = args.This();
	void* ptr = self->GetPointerFromInternalField(0);
  const HeapGraphNode* node = static_cast<HeapSnapshot*>(ptr)->GetNodeById(id);
	return scope.Close(GraphNode::New(node));
}

Handle<Value> Snapshot::GetNode(const Arguments& args) {
	HandleScope scope;
	if (args.Length() < 1) {
    return ThrowException(Exception::Error(String::New("No index specified")));
  } else if (!args[0]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Argument must be integer")));
  }
  int32_t index = args[0]->Int32Value();
  Handle<Object> self = args.This();
	void* ptr = self->GetPointerFromInternalField(0);
  const HeapGraphNode* node = static_cast<HeapSnapshot*>(ptr)->GetNode(index);
	return scope.Close(GraphNode::New(node));
}

Handle<Value> Snapshot::Delete(const Arguments& args) {
	HandleScope scope;
  Handle<Object> self = args.This();
	void* ptr = self->GetPointerFromInternalField(0);
  static_cast<HeapSnapshot*>(ptr)->Delete();
	return Undefined();
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
