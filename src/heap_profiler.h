#ifndef NODE_HEAP_PROFILER_
#define NODE_HEAP_PROFILER_

#include "v8-profiler.h"
#include "node.h"
#include "nan.h"

namespace nodex {
  class HeapProfiler {
    public:
      static void Initialize(v8::Local<v8::Object> exports);

      HeapProfiler();
      virtual ~HeapProfiler();

    protected:
      static void TakeSnapshot(const Nan::FunctionCallbackInfo<v8::Value>& info);
      static void StartTrackingHeapObjects(const Nan::FunctionCallbackInfo<v8::Value>& info);
      static void StopTrackingHeapObjects(const Nan::FunctionCallbackInfo<v8::Value>& info);
      static void GetHeapStats(const Nan::FunctionCallbackInfo<v8::Value>& info);
      static void GetObjectByHeapObjectId(const Nan::FunctionCallbackInfo<v8::Value>& info);
    };
} //namespace nodex

#endif  // NODE_HEAP_PROFILER_H
