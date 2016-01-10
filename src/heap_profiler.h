#ifndef NODE_HEAP_PROFILER_
#define NODE_HEAP_PROFILER_

#include "v8-profiler.h"
#include "node.h"
#include "nan.h"

namespace nodex {
  class HeapProfiler {
    public:
      static void Initialize(v8::Local<v8::Object> target);

      HeapProfiler();
      virtual ~HeapProfiler();

    protected:
      static NAN_METHOD(TakeSnapshot);
      static NAN_METHOD(StartTrackingHeapObjects);
      static NAN_METHOD(StopTrackingHeapObjects);
      static NAN_METHOD(GetHeapStats);
      static NAN_METHOD(GetObjectByHeapObjectId);
      static NAN_METHOD(GetHeapObjectId);
    };
} //namespace nodex

#endif  // NODE_HEAP_PROFILER_H
