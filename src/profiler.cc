#include "node.h"
#include "nan.h"
#include "heap_profiler.h"
#include "cpu_profiler.h"

namespace nodex {
  void InitializeProfiler(v8::Local<v8::Object> exports) {
    Nan::HandleScope scope;
    HeapProfiler::Initialize(exports);
    CpuProfiler::Initialize(exports);
  }

  NODE_MODULE(profiler, InitializeProfiler)
}
