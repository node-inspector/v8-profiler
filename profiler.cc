#include "heap_profiler.h"
#include "cpu_profiler.h"

namespace nodex {
  void InitializeProfiler(Handle<Object> target) {
    HandleScope scope;
    HeapProfiler::Initialize(target);
    CpuProfiler::Initialize(target);
  }

  NODE_MODULE(profiler, InitializeProfiler)
}
