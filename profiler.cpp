#include "profiler.h"
#include "heap_profiler.h"
#include "cpu_profiler.h"

namespace nodex {
	void InitializeProfiler(Handle<ObjectTemplate> target) {
		ENTER_ISOLATE

		HeapProfiler::Initialize(target);
		CpuProfiler::Initialize(target);
	}
}
