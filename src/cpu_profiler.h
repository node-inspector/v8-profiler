#ifndef NODE_CPU_PROFILER_
#define NODE_CPU_PROFILER_

#include "v8-profiler.h"
#include "node.h"
#include "nan.h"

namespace nodex {
  class CpuProfiler {
    public:
      static void Initialize(v8::Local<v8::Object> exports);

      CpuProfiler();
      virtual ~CpuProfiler();

    protected:
      static void StartProfiling(const Nan::FunctionCallbackInfo<v8::Value>& info);
      static void StopProfiling(const Nan::FunctionCallbackInfo<v8::Value>& info);
  };
} //namespace nodex

#endif  // NODE_CPU_PROFILER_H
