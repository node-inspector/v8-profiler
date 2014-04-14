#ifndef NODE_HEAP_PROFILER_
#define NODE_HEAP_PROFILER_

#include <node.h>
#include <v8-profiler.h>
#include "nan.h"

using namespace v8;
using namespace node;

namespace nodex {
    class HeapProfiler {
        public:
            static void Initialize(Handle<Object> target);

            HeapProfiler();
            virtual ~HeapProfiler();

        protected:
            static NAN_METHOD(GetSnapshotsCount);
            static NAN_METHOD(GetSnapshot);
            static NAN_METHOD(FindSnapshot);
            static NAN_METHOD(TakeSnapshot);
            static NAN_METHOD(DeleteAllSnapshots);

        private:
            static Persistent<ObjectTemplate> heap_profiler_template_;
    };
} //namespace nodex

#endif  // NODE_HEAP_PROFILER_H
