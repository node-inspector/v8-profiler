#include "heap_graph_node.h"
#include "heap_graph_edge.h"

namespace nodex {
  using v8::Handle;
  using v8::HeapGraphEdge;
  using v8::Integer;
  using v8::Local;
  using v8::Object;
  using v8::String;
  using v8::Value;

  Handle<Value> GraphEdge::New(const HeapGraphEdge* node) {
    NanEscapableScope();
    
    Local<Object> graph_edge = NanNew<Object>();

    Local<Value> type;
    switch (node->GetType()) {
      case HeapGraphEdge::kContextVariable :
        type = NanNew<String>("ContextVariable");
        break;
      case HeapGraphEdge::kElement :
        type = NanNew<String>("Element");
        break;
      case HeapGraphEdge::kProperty :
        type = NanNew<String>("Property");
        break;
      case HeapGraphEdge::kInternal :
        type = NanNew<String>("Internal");
        break;
      case HeapGraphEdge::kHidden :
        type = NanNew<String>("Hidden");
        break;
      case HeapGraphEdge::kShortcut :
        type = NanNew<String>("Shortcut");
        break;
      case HeapGraphEdge::kWeak :
        type = NanNew<String>("Weak");
        break;
      default :
        type = NanNew<String>("Undefined");
    }
    Handle<Value> name = node->GetName();
    Handle<Value> from = GraphNode::New(node->GetFromNode());
    Handle<Value> to = GraphNode::New(node->GetToNode());

    graph_edge->Set(NanNew<String>("type"), type);
    graph_edge->Set(NanNew<String>("name"), name);
    graph_edge->Set(NanNew<String>("from"), from);
    graph_edge->Set(NanNew<String>("to"), to);

    return NanEscapeScope(graph_edge);
  }
}
