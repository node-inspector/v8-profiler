#include "heap_graph_node.h"
#include "heap_graph_edge.h"

namespace nodex {
  using v8::Array;
  using v8::Handle;
  using v8::HeapGraphNode;
  using v8::Number;
  using v8::Local;
  using v8::Object;
  using v8::ObjectTemplate;
  using v8::FunctionTemplate;
  using v8::String;
  using v8::Value;


  Nan::Persistent<ObjectTemplate> GraphNode::graph_node_template_;
  Nan::Persistent<Object> GraphNode::graph_node_cache;

  NAN_METHOD(GraphNode_EmptyMethod) {
  }

  void GraphNode::Initialize () {
    Nan::HandleScope scope;

    Local<FunctionTemplate> f = Nan::New<FunctionTemplate>(GraphNode_EmptyMethod);
    Local<ObjectTemplate> o = f->InstanceTemplate();
    Local<Object> _cache = Nan::New<Object>();
    o->SetInternalFieldCount(1);
#if (NODE_MODULE_VERSION <= 0x000B)
    Nan::SetMethod(o, "getHeapValue", GraphNode::GetHeapValue);
#endif
    Nan::SetAccessor(o, Nan::New<String>("children").ToLocalChecked(), GraphNode::GetChildren);
    graph_node_template_.Reset(o);
    graph_node_cache.Reset(_cache);
  }

#if (NODE_MODULE_VERSION <= 0x000B)
  NAN_METHOD(GraphNode::GetHeapValue) {
    void* ptr = Nan::GetInternalFieldPointer(info.This(), 0);
    HeapGraphNode* node = static_cast<HeapGraphNode*>(ptr);
    info.GetReturnValue().Set(Nan::New(node->GetHeapValue()));
  }
#endif

  NAN_GETTER(GraphNode::GetChildren) {
    void* ptr = Nan::GetInternalFieldPointer(info.This(), 0);
    HeapGraphNode* node = static_cast<HeapGraphNode*>(ptr);
    uint32_t count = node->GetChildrenCount();
    Local<Array> children = Nan::New<Array>(count);
    for (uint32_t index = 0; index < count; ++index) {
      Local<Value> child = GraphEdge::New(node->GetChild(index));
      children->Set(index, child);
    }
    info.This()->Set(Nan::New<String>("children").ToLocalChecked(), children);
    info.GetReturnValue().Set(children);
  }

  Local<Value> GraphNode::New(const HeapGraphNode* node) {
    Nan::EscapableHandleScope scope;

    if (graph_node_template_.IsEmpty()) {
      GraphNode::Initialize();
    }

    Local<Object> graph_node;
    Local<Object> _cache = Nan::New(graph_node_cache);
    int32_t _id = node->GetId();
    if (_cache->Has(_id)) {
      graph_node = _cache->Get(_id)->ToObject();
    } else {
      graph_node = Nan::New(graph_node_template_)->NewInstance();
      Nan::SetInternalFieldPointer(graph_node, 0, const_cast<HeapGraphNode*>(node));

      Local<Value> type;
      switch (node->GetType()) {
        case HeapGraphNode::kArray :
          type = Nan::New<String>("Array").ToLocalChecked();
          break;
        case HeapGraphNode::kString :
          type = Nan::New<String>("String").ToLocalChecked();
          break;
        case HeapGraphNode::kObject :
          type = Nan::New<String>("Object").ToLocalChecked();
          break;
        case HeapGraphNode::kCode :
          type = Nan::New<String>("Code").ToLocalChecked();
          break;
        case HeapGraphNode::kClosure :
          type = Nan::New<String>("Closure").ToLocalChecked();
          break;
        case HeapGraphNode::kRegExp :
          type = Nan::New<String>("RegExp").ToLocalChecked();
          break;
        case HeapGraphNode::kHeapNumber :
          type = Nan::New<String>("HeapNumber").ToLocalChecked();
          break;
        case HeapGraphNode::kNative :
          type = Nan::New<String>("Native").ToLocalChecked();
          break;
        case HeapGraphNode::kSynthetic :
          type = Nan::New<String>("Synthetic").ToLocalChecked();
          break;
#if (NODE_MODULE_VERSION > 0x000B)
        case HeapGraphNode::kConsString :
          type = Nan::New<String>("ConsString").ToLocalChecked();
          break;
        case HeapGraphNode::kSlicedString :
          type = Nan::New<String>("SlicedString").ToLocalChecked();
          break;
#endif
        default :
          type = Nan::New<String>("Hidden").ToLocalChecked();
      }
#if (NODE_MODULE_VERSION >= 45)
      Local<String> name = node->GetName();
#else
      Local<String> name = Nan::New<String>(node->GetName());
#endif
      Local<Value> id = Nan::New<Number>(_id);
      graph_node->Set(Nan::New<String>("type").ToLocalChecked(), type);
      graph_node->Set(Nan::New<String>("name").ToLocalChecked(), name);
      graph_node->Set(Nan::New<String>("id").ToLocalChecked(), id);

#if (NODE_MODULE_VERSION > 0x000B)
      Local<Value> shallowSize = Nan::New<Number>(node->GetShallowSize());
      graph_node->Set(Nan::New<String>("shallowSize").ToLocalChecked(), shallowSize);
#endif

      _cache->Set(_id, graph_node);
    }

    return scope.Escape(graph_node);
  }
}
