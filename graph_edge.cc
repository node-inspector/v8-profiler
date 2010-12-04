#include "graph_edge.h"
#include "graph_node.h"

using namespace v8;

namespace nodex {
  
Persistent<ObjectTemplate> GraphEdge::edge_template_;

void GraphEdge::Initialize() {
  edge_template_ = Persistent<ObjectTemplate>::New(ObjectTemplate::New());
  edge_template_->SetInternalFieldCount(1);
  edge_template_->SetAccessor(String::New("type"), GraphEdge::GetType);
  edge_template_->SetAccessor(String::New("name"), GraphEdge::GetName);
  edge_template_->SetAccessor(String::New("from"), GraphEdge::GetFrom);
  edge_template_->SetAccessor(String::New("to"), GraphEdge::GetTo);
}

Handle<Value> GraphEdge::GetType(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  int32_t type = static_cast<int32_t>(static_cast<HeapGraphEdge*>(ptr)->GetType());
  Local<String> t;
  switch(type) {
    case 0: //HeapGraphEdge::kContextVariable :
    t = String::New("ContextVariable");
    break;
    case 1: //HeapGraphEdge::kElement :
    t = String::New("Element");
    break;
    case 2: //HeapGraphEdge::kProperty :
    t = String::New("Property");
    break;
    case 3: //HeapGraphEdge::kInternal :
    t = String::New("Internal");
    break;
    case 4: //HeapGraphEdge::kHidden :
    t = String::New("Hidden");
    break;
    case 5: //HeapGraphEdge::kShortcut :
    t = String::New("Shortcut");
    break;
    default:
    t = String::New("Unknown");
    break;
  }
  return scope.Close(t);
}

Handle<Value> GraphEdge::GetName(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  Handle<Value> title = static_cast<HeapGraphEdge*>(ptr)->GetName();
  return scope.Close(title);
}

Handle<Value> GraphEdge::GetFrom(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  const HeapGraphNode* node = static_cast<HeapGraphEdge*>(ptr)->GetFromNode();
  return scope.Close(GraphNode::New(node));
}

Handle<Value> GraphEdge::GetTo(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  const HeapGraphNode* node = static_cast<HeapGraphEdge*>(ptr)->GetToNode();
  return scope.Close(GraphNode::New(node));
}

Handle<Value> GraphEdge::New(const HeapGraphEdge* edge) {
  HandleScope scope;
  
  if (edge_template_.IsEmpty()) {
    GraphEdge::Initialize();
  }
  
  if(!edge) {
    return Undefined();
  }
  else {
    Local<Object> obj = edge_template_->NewInstance();
    obj->SetPointerInInternalField(0, const_cast<HeapGraphEdge*>(edge));
    return scope.Close(obj);
  }
}
}