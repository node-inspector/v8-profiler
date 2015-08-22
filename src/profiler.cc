#include "node.h"
#include "nan.h"
#include "heap_profiler.h"
#include "cpu_profiler.h"

namespace nodex {
  void InitializeProfiler(v8::Local<v8::Object> target) {
    Nan::HandleScope scope;
    HeapProfiler::Initialize(target);
    CpuProfiler::Initialize(target);
  }

  NODE_MODULE(profiler, InitializeProfiler)
}
