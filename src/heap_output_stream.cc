#include "heap_output_stream.h"

namespace nodex {
  using v8::Array;
  using v8::Handle;
  using v8::HeapStatsUpdate;
  using v8::Local;
  using v8::Value;
  using v8::String;
  using v8::OutputStream;
  using v8::Function;
  using v8::TryCatch;;
  using v8::Integer;

  void OutputStreamAdapter::EndOfStream() {
    TryCatch try_catch;
    callback->Call(NanGetCurrentContext()->Global(), 0, NULL);

    if (try_catch.HasCaught()) {
      NanThrowError(try_catch.Exception());
    }
  }
      
  int OutputStreamAdapter::GetChunkSize() {
    return 51200;
  }

  OutputStream::WriteResult OutputStreamAdapter::WriteAsciiChunk(char* data, int size) {
    NanScope();

    Handle<Value> argv[2] = {
      NanNew<String>(data, size),
      NanNew<Integer>(size)
    };

    TryCatch try_catch;
    abort = iterator->Call(NanGetCurrentContext()->Global(), 2, argv);

    if (try_catch.HasCaught()) {
      NanThrowError(try_catch.Exception());
      return kAbort;
    }
    
    return abort->IsFalse() ? kAbort : kContinue;
  }
  
  OutputStream::WriteResult OutputStreamAdapter::WriteHeapStatsChunk(HeapStatsUpdate* data, int count) {
    NanScope();
    
    Local<Array> samples = NanNew<Array>();
    for (int index = 0; index < count; index++) {
      int offset = index * 3;
      samples->Set(offset, NanNew<Integer>(data[index].index));
      samples->Set(offset+1, NanNew<Integer>(data[index].count));
      samples->Set(offset+2, NanNew<Integer>(data[index].size));
    }

    Local<Value> argv[1] = {samples};

    TryCatch try_catch;
    abort = iterator->Call(NanGetCurrentContext()->Global(), 1, argv);

    if (try_catch.HasCaught()) {
      NanThrowError(try_catch.Exception());
      return kAbort;
    }

    return abort->IsFalse() ? kAbort : kContinue;
  }
} //namespace nodex