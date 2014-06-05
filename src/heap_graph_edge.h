#ifndef NODE_GRAPH_EDGE_
#define NODE_GRAPH_EDGE_

#include "v8-profiler.h"
#include "nan.h"

namespace nodex {

  class GraphEdge {
    public:
      static v8::Handle<v8::Value> New(const v8::HeapGraphEdge* node);
  };
} //namespace nodex
#endif  // NODE_GRAPH_EDGE_
