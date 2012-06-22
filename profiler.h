// I wrote this one from scratch -cat

#ifndef PROFILER_H
#define PROFILER_H

#include <v8-profiler.h>

using namespace v8;

namespace nodex {


  void InitializeProfiler(Handle<ObjectTemplate> target);


} // namespace nodex

#endif // PROFILER_H

