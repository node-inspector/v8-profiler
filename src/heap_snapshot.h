#ifndef NODE_SNAPSHOT_
#define NODE_SNAPSHOT_

#include "v8-profiler.h"
#include "nan.h"

namespace nodex {

  class Snapshot {
    public:
      static v8::Local<v8::Value> New(const v8::HeapSnapshot* node);
      static Nan::Persistent<v8::Array> snapshots;
    private:
      static void Initialize();
      static NAN_GETTER(GetRoot);
      static void GetNode(const Nan::FunctionCallbackInfo<v8::Value>& info);
      static void GetNodeById(const Nan::FunctionCallbackInfo<v8::Value>& info);
      static void Delete(const Nan::FunctionCallbackInfo<v8::Value>& info);
      static void Serialize(const Nan::FunctionCallbackInfo<v8::Value>& info);
      static Nan::Persistent<v8::ObjectTemplate> snapshot_template_;
  };
} //namespace nodex
#endif  // NODE_SNAPSHOT_
