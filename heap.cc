#include <v8.h>
#include <v8-profiler.h>
#include <node.h>
#include "snapshot.h"

using namespace v8;
using namespace node;
using namespace nodex;

static Handle<Value> TakeSnapshot(const Arguments& args) {
  HandleScope scope;
  Local<String> title = String::New("");
  if (args.Length() > 0) {
    title = args[0]->ToString();
  }
  const HeapSnapshot* snapshot = HeapProfiler::TakeSnapshot(title);
  return scope.Close(Snapshot::New(snapshot));
}

static Handle<Value> GetSnapshot(const Arguments& args) {
  HandleScope scope;
  if (args.Length() < 1) {
    return ThrowException(Exception::Error(String::New("No index specified")));
  } else if (!args[0]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Argument must be integer")));
  }
  int32_t index = args[0]->Int32Value();
  const HeapSnapshot* snapshot = HeapProfiler::GetSnapshot(index);
  return scope.Close(Snapshot::New(snapshot));
}

static Handle<Value> FindSnapshot(const Arguments& args) {
  HandleScope scope;
  if (args.Length() < 1) {
    return ThrowException(Exception::Error(String::New("No uid specified")));
  }
  uint32_t uid = args[0]->Uint32Value();
  const HeapSnapshot* snapshot = HeapProfiler::FindSnapshot(uid);
  return scope.Close(Snapshot::New(snapshot));
}

static Handle<Value> GetSnapshotsCount(const Arguments& args) {
  HandleScope scope;
  return scope.Close(Integer::New(HeapProfiler::GetSnapshotsCount()));
}

extern "C" void init(Handle<Object> target) {
  HandleScope scope;

  NODE_SET_METHOD(target, "takeSnapshot", TakeSnapshot);
  NODE_SET_METHOD(target, "getSnapshot", GetSnapshot);
  NODE_SET_METHOD(target, "findSnapshot", FindSnapshot);
  NODE_SET_METHOD(target, "snapshotCount", GetSnapshotsCount);
}