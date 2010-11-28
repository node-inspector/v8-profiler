

#ifndef NODE_SNAPSHOT_DIFF_
#define NODE_SNAPSHOT_DIFF_

#include <v8.h>
#include <v8-profiler.h>

using namespace v8;

namespace nodex {

class SnapshotDiff {
 public:
   static Handle<Value> New(const HeapSnapshotsDiff* diff);

 private:
   static Handle<Value> GetAdditions(Local<String> property, const AccessorInfo& info);
   static Handle<Value> GetDeletions(Local<String> property, const AccessorInfo& info);
   static void Initialize();
   static Persistent<ObjectTemplate> diff_template_;
};

}
#endif  // NODE_SNAPSHOT_DIFF_