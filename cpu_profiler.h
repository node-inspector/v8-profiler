#ifndef NODE_CPU_PROFILER_
#define NODE_CPU_PROFILER_

#include <v8-profiler.h>
#include <node.h>

using namespace v8;
using namespace node;

namespace nodex {
    class CpuProfiler {
        public:
            static void Initialize(Handle<Object> target);

            CpuProfiler();
            virtual ~CpuProfiler();

        protected:
            static Handle<Value> GetProfilesCount(const Arguments& args);
            static Handle<Value> GetProfile(const Arguments& args);
            static Handle<Value> FindProfile(const Arguments& args);
            static Handle<Value> StartProfiling(const Arguments& args);
            static Handle<Value> StopProfiling(const Arguments& args);
            static Handle<Value> DeleteAllProfiles(const Arguments& args);

        private:
            static Persistent<ObjectTemplate> cpu_profiler_template_;
    };
} //namespace nodex

#endif  // NODE_CPU_PROFILER_H
