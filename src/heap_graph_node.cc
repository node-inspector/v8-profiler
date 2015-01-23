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
  using v8::Persistent;
  using v8::String;
  using v8::Value;
  

  Persistent<ObjectTemplate> GraphNode::graph_node_template_;
  Persistent<Object> GraphNode::graph_node_cache;

  void GraphNode::Initialize () {
    NanScope();
    
    Local<FunctionTemplate> f = NanNew<FunctionTemplate>();
    Local<ObjectTemplate> o = f->InstanceTemplate();
    Local<Object> _cache = NanNew<Object>();
    o->SetInternalFieldCount(1);
#if (NODE_MODULE_VERSION <= 0x000B)
    NODE_SET_METHOD(o, "getHeapValue", GraphNode::GetHeapValue);
#endif
    o->SetAccessor(NanNew<String>("children"), GraphNode::GetChildren);
    NanAssignPersistent(graph_node_template_, o);
    NanAssignPersistent(graph_node_cache, _cache);
  }

#if (NODE_MODULE_VERSION <= 0x000B)
  NAN_METHOD(GraphNode::GetHeapValue) {
    NanScope();

    void* ptr = NanGetInternalFieldPointer(args.This(), 0);
    HeapGraphNode* node = static_cast<HeapGraphNode*>(ptr);
    NanReturnValue(node->GetHeapValue());
  }
#endif
  
  NAN_GETTER(GraphNode::GetChildren) {
    NanScope();

    void* ptr = NanGetInternalFieldPointer(args.This(), 0);
    HeapGraphNode* node = static_cast<HeapGraphNode*>(ptr);
    uint32_t count = node->GetChildrenCount();
    Local<Array> children = NanNew<Array>(count);
    for (uint32_t index = 0; index < count; ++index) {
      Handle<Value> child = GraphEdge::New(node->GetChild(index));
      children->Set(index, child);
    }
    args.This()->Set(NanNew<String>("children"), children);
    NanReturnValue(children);
  }

  Handle<Value> GraphNode::New(const HeapGraphNode* node) {
    NanEscapableScope();

    if (graph_node_template_.IsEmpty()) {
      GraphNode::Initialize();
    }

    Local<Object> graph_node;
    Local<Object> _cache = NanNew(graph_node_cache);
    int32_t _id = node->GetId();
    if (_cache->Has(_id)) {
      graph_node = _cache->Get(_id)->ToObject();
    } else {
      graph_node = NanNew(graph_node_template_)->NewInstance();
      NanSetInternalFieldPointer(graph_node, 0, const_cast<HeapGraphNode*>(node));

      Local<Value> type;
      switch (node->GetType()) {
        case HeapGraphNode::kArray :
          type = NanNew<String>("Array");
          break;
        case HeapGraphNode::kString :
          type = NanNew<String>("String");
          break;
        case HeapGraphNode::kObject :
          type = NanNew<String>("Object");
          break;  
        case HeapGraphNode::kCode :
          type = NanNew<String>("Code");
          break;
        case HeapGraphNode::kClosure :
          type = NanNew<String>("Closure");
          break;
        case HeapGraphNode::kRegExp :
          type = NanNew<String>("RegExp");
          break;
        case HeapGraphNode::kHeapNumber :
          type = NanNew<String>("HeapNumber");
          break;
        case HeapGraphNode::kNative :
          type = NanNew<String>("Native");
          break;
        case HeapGraphNode::kSynthetic :
          type = NanNew<String>("Synthetic");
          break;
#if (NODE_MODULE_VERSION > 0x000B)
        case HeapGraphNode::kConsString :
          type = NanNew<String>("ConsString");
          break;
        case HeapGraphNode::kSlicedString :
          type = NanNew<String>("SlicedString");
          break;
#endif
        default :
          type = NanNew<String>("Hidden");
      }
      Handle<Value> name = node->GetName();
      Local<Value> id = NanNew<Number>(_id);
      graph_node->Set(NanNew<String>("type"), type);
      graph_node->Set(NanNew<String>("name"), name);
      graph_node->Set(NanNew<String>("id"), id);
      
#if (NODE_MODULE_VERSION > 0x000B)
      Handle<Value> shallowSize = NanNew<Number>(node->GetShallowSize());
      graph_node->Set(NanNew<String>("shallowSize"), shallowSize);
#endif

      _cache->Set(_id, graph_node);
    }

    return NanEscapeScope(graph_node);
  }
}
