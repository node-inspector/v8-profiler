

#ifndef NODE_SNAPSHOT_
#define NODE_SNAPSHOT_

#include <node.h>
#include <v8-profiler.h>
#include "nan.h"

using namespace v8;

namespace nodex {

class Snapshot {
  public:
    static Handle<Value> New(const HeapSnapshot* snapshot);

  private:
    Snapshot(const v8::HeapSnapshot* snapshot)
        : m_snapshot(snapshot){}

    const v8::HeapSnapshot* m_snapshot;

    static NAN_GETTER(GetUid);
    static NAN_GETTER(GetTitle);
    static NAN_GETTER(GetMaxSnapshotJSObjectId);
    static NAN_GETTER(GetRoot);
    static NAN_GETTER(GetType);
    static NAN_GETTER(GetNodesCount);
    static NAN_METHOD(GetNodeById);
    static NAN_METHOD(GetNode);
    static NAN_METHOD(Delete);
    static NAN_METHOD(Serialize);

    static NAN_METHOD(SnapshotObjectId);

    static void Initialize();
    static Persistent<ObjectTemplate> snapshot_template_;
};
} //namespace nodex
#endif  // NODE_SNAPSHOT_
