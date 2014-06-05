#ifndef NODE_HEAP_OUTPUT_STREAM_
#define NODE_HEAP_OUTPUT_STREAM_

#include "v8-profiler.h"
#include "nan.h"

namespace nodex {

class OutputStreamAdapter : public v8::OutputStream {
    public:
      OutputStreamAdapter(
        v8::Handle<v8::Function> _iterator, 
        v8::Handle<v8::Function> _callback)
      : iterator(_iterator)
      , callback(_callback)
      , abort(NanFalse()) {};
      
      void EndOfStream();
      
      int GetChunkSize();

      WriteResult WriteAsciiChunk(char* data, int size);

      WriteResult WriteHeapStatsChunk(v8::HeapStatsUpdate* data, int count);
    private:
      v8::Handle<v8::Value> abort;
      v8::Handle<v8::Function> iterator;
      v8::Handle<v8::Function> callback;
  };
} //namespace nodex
#endif  // NODE_PROFILE_
