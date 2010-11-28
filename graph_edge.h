

#ifndef NODE_GRAPH_EDGE_
#define NODE_GRAPH_EDGE_

#include <v8.h>
#include <v8-profiler.h>

using namespace v8;

namespace nodex {

class GraphEdge {
 public:
   static Handle<Value> New(const HeapGraphEdge* edge);

 private:
   static Handle<Value> GetType(Local<String> property, const AccessorInfo& info);
   static Handle<Value> GetName(Local<String> property, const AccessorInfo& info);
   static Handle<Value> GetFrom(Local<String> property, const AccessorInfo& info);
   static Handle<Value> GetTo(Local<String> property, const AccessorInfo& info);
   static void Initialize();
   static Persistent<ObjectTemplate> edge_template_;
};

}
#endif  // NODE_GRAPH_EDGE_