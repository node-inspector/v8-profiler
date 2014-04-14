

#ifndef NODE_SNAPSHOT_
#define NODE_SNAPSHOT_

#include <node.h>
#include <v8-profiler.h>

using namespace v8;

namespace nodex {

class Snapshot {
  public:
    static Handle<Value> New(const HeapSnapshot* snapshot);

  private:
    Snapshot(const v8::HeapSnapshot* snapshot)
        : m_snapshot(snapshot){}

    const v8::HeapSnapshot* m_snapshot;

    static Handle<Value> GetUid(Local<String> property, const AccessorInfo& info);
    static Handle<Value> GetTitle(Local<String> property, const AccessorInfo& info);
    static Handle<Value> GetMaxSnapshotJSObjectId(Local<String> property, const AccessorInfo& info);
    static Handle<Value> GetRoot(Local<String> property, const AccessorInfo& info);
    static Handle<Value> GetType(Local<String> property, const AccessorInfo& info);
    static Handle<Value> GetNodesCount(Local<String> property, const AccessorInfo& info);
    static Handle<Value> GetNodeById(const Arguments& args);
    static Handle<Value> GetNode(const Arguments& args);
    static Handle<Value> Delete(const Arguments& args);
    static Handle<Value> Serialize(const Arguments& args);

    static Handle<Value> SnapshotObjectId(const Arguments& args);

    static void Initialize();
    static Persistent<ObjectTemplate> snapshot_template_;
};
} //namespace nodex
#endif  // NODE_SNAPSHOT_
