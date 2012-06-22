#include "profile_node.h"

using namespace v8;

namespace nodex {

Persistent<ObjectTemplate> ProfileNode::node_template_;

void ProfileNode::Initialize() {
  node_template_ = Persistent<ObjectTemplate>::New(ObjectTemplate::New());
  node_template_->SetInternalFieldCount(1);
  node_template_->SetAccessor(String::New("functionName"), ProfileNode::GetFunctionName);
  node_template_->SetAccessor(String::New("scriptName"), ProfileNode::GetScriptName);
  node_template_->SetAccessor(String::New("lineNumber"), ProfileNode::GetLineNumber);
  node_template_->SetAccessor(String::New("totalTime"), ProfileNode::GetTotalTime);
  node_template_->SetAccessor(String::New("selfTime"), ProfileNode::GetSelfTime);
  node_template_->SetAccessor(String::New("totalSamplesCount"), ProfileNode::GetTotalSamplesCount);
  node_template_->SetAccessor(String::New("selfSamplesCount"), ProfileNode::GetSelfSamplesCount);
  node_template_->SetAccessor(String::New("callUid"), ProfileNode::GetCallUid);
  node_template_->SetAccessor(String::New("childrenCount"), ProfileNode::GetChildrenCount);
  node_template_->Set(String::New("getChild"), FunctionTemplate::New(ProfileNode::GetChild));
}

Handle<Value> ProfileNode::GetFunctionName(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  Handle<String> fname = static_cast<CpuProfileNode*>(ptr)->GetFunctionName();
  return scope.Close(fname);
}

Handle<Value> ProfileNode::GetScriptName(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  Handle<String> sname = static_cast<CpuProfileNode*>(ptr)->GetScriptResourceName();
  return scope.Close(sname);
}

Handle<Value> ProfileNode::GetLineNumber(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  int32_t ln = static_cast<CpuProfileNode*>(ptr)->GetLineNumber();
  return scope.Close(Integer::New(ln));
}

Handle<Value> ProfileNode::GetTotalTime(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  double ttime = static_cast<CpuProfileNode*>(ptr)->GetTotalTime();
  return scope.Close(Number::New(ttime));
}

Handle<Value> ProfileNode::GetSelfTime(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  double stime = static_cast<CpuProfileNode*>(ptr)->GetSelfTime();
  return scope.Close(Number::New(stime));
}

Handle<Value> ProfileNode::GetTotalSamplesCount(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  double samples = static_cast<CpuProfileNode*>(ptr)->GetTotalSamplesCount();
  return scope.Close(Number::New(samples));
}

Handle<Value> ProfileNode::GetSelfSamplesCount(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  double samples = static_cast<CpuProfileNode*>(ptr)->GetSelfSamplesCount();
  return scope.Close(Number::New(samples));
}

Handle<Value> ProfileNode::GetCallUid(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  uint32_t uid = static_cast<CpuProfileNode*>(ptr)->GetCallUid();
  return scope.Close(Integer::NewFromUnsigned(uid));
}

Handle<Value> ProfileNode::GetChildrenCount(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  int32_t count = static_cast<CpuProfileNode*>(ptr)->GetChildrenCount();
  return scope.Close(Integer::New(count));
}

Handle<Value> ProfileNode::GetChild(const Arguments& args) {
  HandleScope scope;
  if (args.Length() < 1) {
    return ThrowException(Exception::Error(String::New("No index specified")));
  } else if (!args[0]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Argument must be integer")));
  }
  int32_t index = args[0]->Int32Value();
  Handle<Object> self = args.This();
  void* ptr = self->GetPointerFromInternalField(0);
  const CpuProfileNode* node = static_cast<CpuProfileNode*>(ptr)->GetChild(index);
  return scope.Close(ProfileNode::New(node));
}

Handle<Value> ProfileNode::New(const CpuProfileNode* node) {
  HandleScope scope;
  
  if (node_template_.IsEmpty()) {
    ProfileNode::Initialize();
  }
  
  if(!node) {
    return Undefined();
  }
  else {
    Local<Object> obj = node_template_->NewInstance();
    obj->SetPointerInInternalField(0, const_cast<CpuProfileNode*>(node));
    return scope.Close(obj);
  }
}
}