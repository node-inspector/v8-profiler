#ifndef NODE_HEAP_PROFILER_
#define NODE_HEAP_PROFILER_

#include <node.h>
#include <v8-profiler.h>

using namespace v8;
using namespace node;

namespace nodex {
    class HeapProfiler {
        public:
            static void Initialize(Handle<Object> target);

            HeapProfiler();
            virtual ~HeapProfiler();

        protected:
            static Handle<Value> GetSnapshotsCount(const Arguments& args);
            static Handle<Value> GetSnapshot(const Arguments& args);
            static Handle<Value> FindSnapshot(const Arguments& args);
            static Handle<Value> TakeSnapshot(const Arguments& args);
            static Handle<Value> DeleteAllSnapshots(const Arguments& args);

        private:
            static Persistent<ObjectTemplate> heap_profiler_template_;
    };
} //namespace nodex

#endif  // NODE_HEAP_PROFILER_H
