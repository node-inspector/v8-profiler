

#ifndef NODE_SNAPSHOT_
#define NODE_SNAPSHOT_

#include <v8.h>
#include <v8-profiler.h>

using namespace v8;

namespace nodex {

class Snapshot {
 public:
   static Handle<Value> New(const HeapSnapshot* snapshot);

 private:
   static Handle<Value> GetUid(Local<String> property, const AccessorInfo& info);
   static Handle<Value> GetTitle(Local<String> property, const AccessorInfo& info);
   static Handle<Value> GetRoot(Local<String> property, const AccessorInfo& info);
   static Handle<Value> GetType(Local<String> property, const AccessorInfo& info);
   static Handle<Value> CompareWith(const Arguments& args);
   static void Initialize();
   static Persistent<ObjectTemplate> snapshot_template_;
};

}
#endif  // NODE_SNAPSHOT_