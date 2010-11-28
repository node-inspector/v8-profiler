#include "graph_path.h"
#include "graph_node.h"
#include "graph_edge.h"

using namespace v8;

namespace nodex {
  
Persistent<ObjectTemplate> GraphPath::path_template_;

void GraphPath::Initialize() {
  path_template_ = Persistent<ObjectTemplate>::New(ObjectTemplate::New());
  path_template_->SetInternalFieldCount(1);
  path_template_->SetAccessor(String::New("edgesCount"), GraphPath::GetEdgesCount);
  path_template_->SetAccessor(String::New("from"), GraphPath::GetFromNode);
  path_template_->SetAccessor(String::New("to"), GraphPath::GetToNode);
  path_template_->Set(String::New("getEdge"), FunctionTemplate::New(GraphPath::GetEdge));
}
  
Handle<Value> GraphPath::GetEdgesCount(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  int32_t count = static_cast<HeapGraphPath*>(ptr)->GetEdgesCount();
  return scope.Close(Integer::New(count));
}

Handle<Value> GraphPath::GetEdge(const Arguments& args) {
  HandleScope scope;
  if (args.Length() < 1) {
    return ThrowException(Exception::Error(String::New("No index specified")));
  } else if (!args[0]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Argument must be integer")));
  }
  int32_t index = args[0]->Int32Value();
  Handle<Object> self = args.This();
  void* ptr = self->GetPointerFromInternalField(0);
  const HeapGraphEdge* edge = static_cast<HeapGraphPath*>(ptr)->GetEdge(index);
  return scope.Close(GraphEdge::New(edge));
}

Handle<Value> GraphPath::GetFromNode(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  const HeapGraphNode* node = static_cast<HeapGraphPath*>(ptr)->GetFromNode();
  return scope.Close(GraphNode::New(node));
}

Handle<Value> GraphPath::GetToNode(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  const HeapGraphNode* node = static_cast<HeapGraphPath*>(ptr)->GetToNode();
  return scope.Close(GraphNode::New(node));
}

Handle<Value> GraphPath::New(const HeapGraphPath* path) {
  HandleScope scope;
  
  if (path_template_.IsEmpty()) {
    GraphPath::Initialize();
  }
  
  if(!path) {
    return Undefined();
  }
  else {
    Local<Object> obj = path_template_->NewInstance();
    obj->SetPointerInInternalField(0, const_cast<HeapGraphPath*>(path));
    return scope.Close(obj);
  }
}
}