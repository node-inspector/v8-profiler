#include "heap_profiler.h"
#include "cpu_profiler.h"

namespace nodex {
  void InitializeProfiler(Handle<ObjectTemplate> target) {
    HandleScope scope;
    HeapProfiler::Initialize(target);
    CpuProfiler::Initialize(target);
  }

  // Not using node.js
  //NODE_MODULE(profiler, InitializeProfiler)
}
