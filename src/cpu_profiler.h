#ifndef NODE_CPU_PROFILER_
#define NODE_CPU_PROFILER_

#include <node.h>
#include <v8-profiler.h>
#include "nan.h"

using namespace v8;
using namespace node;

namespace nodex {
    class CpuProfiler {
        public:
            static void Initialize(Handle<Object> target);

            CpuProfiler();
            virtual ~CpuProfiler();

        protected:
            static NAN_METHOD(GetProfilesCount);
            static NAN_METHOD(GetProfile);
            static NAN_METHOD(FindProfile);
            static NAN_METHOD(StartProfiling);
            static NAN_METHOD(StopProfiling);
            static NAN_METHOD(DeleteAllProfiles);

        private:
            static Persistent<ObjectTemplate> cpu_profiler_template_;
    };
} //namespace nodex

#endif  // NODE_CPU_PROFILER_H
