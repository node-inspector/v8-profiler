

#ifndef NODE_GRAPH_NODE_
#define NODE_GRAPH_NODE_

#include <v8.h>
#include <v8-profiler.h>

using namespace v8;

namespace nodex {

class GraphNode {
 public:
   static Handle<Value> New(const HeapGraphNode* node);

 private:
   static Handle<Value> GetType(Local<String> property, const AccessorInfo& info);
   static Handle<Value> GetName(Local<String> property, const AccessorInfo& info);
   static Handle<Value> GetId(Local<String> property, const AccessorInfo& info);
   static Handle<Value> GetPtr(Local<String> property, const AccessorInfo& info);
   static Handle<Value> GetInstancesCount(Local<String> property, const AccessorInfo& info);
   static Handle<Value> GetChildrenCount(Local<String> property, const AccessorInfo& info);
   static Handle<Value> GetRetainersCount(Local<String> property, const AccessorInfo& info);
   static Handle<Value> GetSize(Local<String> property, const AccessorInfo& info);
   static Handle<Value> GetChild(const Arguments& args);
   static Handle<Value> GetRetainedSize(const Arguments& args);
   static Handle<Value> GetRetainer(const Arguments& args);
   static Handle<Value> GetRetainingPathsCount(Local<String> property, const AccessorInfo& info);
   static Handle<Value> GetRetainingPath(const Arguments& args);
   static Handle<Value> GetDominator(Local<String> property, const AccessorInfo& info);
   static void Initialize();
   static Persistent<ObjectTemplate> node_template_;
};

}
#endif  // NODE_GRAPH_NODE_