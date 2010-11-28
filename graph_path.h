

#ifndef NODE_GRAPH_PATH_
#define NODE_GRAPH_PATH_

#include <v8.h>
#include <v8-profiler.h>

using namespace v8;

namespace nodex {

class GraphPath {
 public:
   static Handle<Value> New(const HeapGraphPath* path);

 private:
   static Handle<Value> GetEdgesCount(Local<String> property, const AccessorInfo& info);
   static Handle<Value> GetToNode(Local<String> property, const AccessorInfo& info);
   static Handle<Value> GetFromNode(Local<String> property, const AccessorInfo& info);
   static Handle<Value> GetEdge(const Arguments& args);
   static void Initialize();
   static Persistent<ObjectTemplate> path_template_;
};

}
#endif  // NODE_GRAPH_PATH_