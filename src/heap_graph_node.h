#ifndef NODE_GRAPH_NODE_
#define NODE_GRAPH_NODE_

#include "v8-profiler.h"
#include "nan.h"

namespace nodex {

  class GraphNode {
    public:
      static v8::Handle<v8::Value> New(const v8::HeapGraphNode* node);
    private:
      static void Initialize();
      static NAN_METHOD(GetHeapValue);
      static NAN_GETTER(GetChildren);
      static v8::Persistent<v8::ObjectTemplate> graph_node_template_;
      static v8::Persistent<v8::Object> graph_node_cache;
  };
} //namespace nodex
#endif  // NODE_GRAPH_NODE_
