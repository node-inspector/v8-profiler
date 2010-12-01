#include <v8.h>
#include <v8-profiler.h>
#include <node.h>
#include "snapshot.h"
#include "profile.h"

using namespace v8;
using namespace node;
using namespace nodex;

static Handle<Value> TakeSnapshot(const Arguments& args) {
  HandleScope scope;
  Local<String> title = String::New("");
  int32_t len = args.Length();
  HeapSnapshot::Type mode = HeapSnapshot::kAggregated;
  if (len > 0) {
    title = args[0]->ToString();
  }
  if (len > 1 && args[1]->IsInt32()) {
    mode = static_cast<HeapSnapshot::Type>(args[1]->Int32Value());
  }
  const HeapSnapshot* snapshot = HeapProfiler::TakeSnapshot(title, mode);
  return scope.Close(Snapshot::New(snapshot));
}

static Handle<Value> GetSnapshot(const Arguments& args) {
  HandleScope scope;
  if (args.Length() < 1) {
    return ThrowException(Exception::Error(String::New("No index specified")));
  } else if (!args[0]->IsInt32()) {
    return ThrowException(Exception::TypeError(String::New("Argument must be an integer")));
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

Handle<Value> StartProfiling(const Arguments& args) {
  HandleScope scope;
  Local<String> title = args.Length() > 0 ? args[0]->ToString() : String::New("");
  v8::CpuProfiler::StartProfiling(title);
  return Undefined();
}

Handle<Value> StopProfiling(const Arguments& args) {
  HandleScope scope;
  Local<String> title = args.Length() > 0 ? args[0]->ToString() : String::New("");
  const CpuProfile* profile = v8::CpuProfiler::StopProfiling(title);
  return scope.Close(Profile::New(profile));
}

Handle<Value> GetProfile(const Arguments& args) {
  HandleScope scope;
  if (args.Length() < 1) {
    return ThrowException(Exception::Error(String::New("No index specified")));
  } else if (!args[0]->IsInt32()) {
    return ThrowException(Exception::TypeError(String::New("Argument must be an integer")));
  }
  int32_t index = args[0]->Int32Value();
  const CpuProfile* profile = v8::CpuProfiler::GetProfile(index);
  return scope.Close(Profile::New(profile));
}

Handle<Value> FindProfile(const Arguments& args) {
  HandleScope scope;
  if (args.Length() < 1) {
    return ThrowException(Exception::Error(String::New("No index specified")));
  } else if (!args[0]->IsInt32()) {
    return ThrowException(Exception::TypeError(String::New("Argument must be an integer")));
  }
  uint32_t uid = args[0]->Uint32Value();
  const CpuProfile* profile = v8::CpuProfiler::FindProfile(uid);
  return scope.Close(Profile::New(profile));
}

Handle<Value> GetProfilesCount(const Arguments& args) {
  HandleScope scope;
  return scope.Close(Integer::New(v8::CpuProfiler::GetProfilesCount()));
}

extern "C" void init(Handle<Object> target) {
  HandleScope scope;

  NODE_SET_METHOD(target, "takeSnapshot", TakeSnapshot);
  NODE_SET_METHOD(target, "getSnapshot", GetSnapshot);
  NODE_SET_METHOD(target, "findSnapshot", FindSnapshot);
  NODE_SET_METHOD(target, "snapshotCount", GetSnapshotsCount);
  
  NODE_SET_METHOD(target, "startProfiling", StartProfiling);
  NODE_SET_METHOD(target, "stopProfiling", StopProfiling);
  NODE_SET_METHOD(target, "getProfile", GetProfile);
  NODE_SET_METHOD(target, "findProfile", FindProfile);
  NODE_SET_METHOD(target, "getProfilesCount", GetProfilesCount);
}