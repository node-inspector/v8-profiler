#ifndef NODE_SNAPSHOT_
#define NODE_SNAPSHOT_

#include "v8-profiler.h"
#include "nan.h"

namespace nodex {

  class Snapshot {
    public:
      static v8::Handle<v8::Value> New(const v8::HeapSnapshot* node);
      static v8::Persistent<v8::Array> snapshots;
    private:
      static void Initialize();
      static NAN_GETTER(GetRoot);
      static NAN_METHOD(GetNode);
      static NAN_METHOD(GetNodeById);
      static NAN_METHOD(Delete);
      static NAN_METHOD(Serialize);
      static v8::Persistent<v8::ObjectTemplate> snapshot_template_;
  };
} //namespace nodex
#endif  // NODE_SNAPSHOT_
