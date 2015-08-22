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

  Local<Value> GraphEdge::New(const HeapGraphEdge* node) {
    Nan::EscapableHandleScope scope;

    Local<Object> graph_edge = Nan::New<Object>();

    Local<Value> type;
    switch (node->GetType()) {
      case HeapGraphEdge::kContextVariable :
        type = Nan::New<String>("ContextVariable").ToLocalChecked();
        break;
      case HeapGraphEdge::kElement :
        type = Nan::New<String>("Element").ToLocalChecked();
        break;
      case HeapGraphEdge::kProperty :
        type = Nan::New<String>("Property").ToLocalChecked();
        break;
      case HeapGraphEdge::kInternal :
        type = Nan::New<String>("Internal").ToLocalChecked();
        break;
      case HeapGraphEdge::kHidden :
        type = Nan::New<String>("Hidden").ToLocalChecked();
        break;
      case HeapGraphEdge::kShortcut :
        type = Nan::New<String>("Shortcut").ToLocalChecked();
        break;
      case HeapGraphEdge::kWeak :
        type = Nan::New<String>("Weak").ToLocalChecked();
        break;
      default :
        type = Nan::New<String>("Undefined").ToLocalChecked();
    }
#if (NODE_MODULE_VERSION >= 45)
    Local<Value> name = node->GetName();
#else
    Local<Value> name = Nan::New<Value>(node->GetName());
#endif
    Local<Value> from = GraphNode::New(node->GetFromNode());
    Local<Value> to = GraphNode::New(node->GetToNode());

    graph_edge->Set(Nan::New<String>("type").ToLocalChecked(), type);
    graph_edge->Set(Nan::New<String>("name").ToLocalChecked(), name);
    graph_edge->Set(Nan::New<String>("from").ToLocalChecked(), from);
    graph_edge->Set(Nan::New<String>("to").ToLocalChecked(), to);

    return scope.Escape(graph_edge);
  }
}
