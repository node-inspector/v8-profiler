#include "profile_node.h"

using namespace v8;

namespace nodex {

Persistent<ObjectTemplate> ProfileNode::node_template_;

void ProfileNode::Initialize() {
  Local<ObjectTemplate> tpl = NanNewLocal<ObjectTemplate>(ObjectTemplate::New());
  NanAssignPersistent(ObjectTemplate, node_template_, tpl);
  tpl->SetInternalFieldCount(1);
  tpl->SetAccessor(String::New("functionName"), ProfileNode::GetFunctionName);
  tpl->SetAccessor(String::New("scriptName"), ProfileNode::GetScriptName);
  tpl->SetAccessor(String::New("lineNumber"), ProfileNode::GetLineNumber);
  tpl->SetAccessor(String::New("totalTime"), ProfileNode::GetTotalTime);
  tpl->SetAccessor(String::New("selfTime"), ProfileNode::GetSelfTime);
  tpl->SetAccessor(String::New("totalSamplesCount"), ProfileNode::GetTotalSamplesCount);
  tpl->SetAccessor(String::New("selfSamplesCount"), ProfileNode::GetSelfSamplesCount);
  tpl->SetAccessor(String::New("callUid"), ProfileNode::GetCallUid);
  tpl->SetAccessor(String::New("childrenCount"), ProfileNode::GetChildrenCount);
  tpl->Set(String::New("getChild"), FunctionTemplate::New(ProfileNode::GetChild));
}

NAN_GETTER(ProfileNode::GetFunctionName) {
  NanScope();
  Local<Object> self = args.Holder();
  void* ptr = NanGetInternalFieldPointer(self, 0);
  Handle<String> fname = static_cast<CpuProfileNode*>(ptr)->GetFunctionName();
  NanReturnValue(fname);
}

NAN_GETTER(ProfileNode::GetScriptName) {
  NanScope();
  Local<Object> self = args.Holder();
  void* ptr = NanGetInternalFieldPointer(self, 0);
  Handle<String> sname = static_cast<CpuProfileNode*>(ptr)->GetScriptResourceName();
  NanReturnValue(sname);
}

NAN_GETTER(ProfileNode::GetLineNumber) {
  NanScope();
  Local<Object> self = args.Holder();
  void* ptr = NanGetInternalFieldPointer(self, 0);
  int32_t ln = static_cast<CpuProfileNode*>(ptr)->GetLineNumber();
  NanReturnValue(Integer::New(ln));
}

NAN_GETTER(ProfileNode::GetTotalTime) {
  NanScope();
#if (NODE_MODULE_VERSION > 0x000B)
  NanReturnUndefined();
#else
  Local<Object> self = args.Holder();
  void* ptr = NanGetInternalFieldPointer(self, 0);
  double ttime = static_cast<CpuProfileNode*>(ptr)->GetTotalTime();
  NanReturnValue(Number::New(ttime));
#endif
}

NAN_GETTER(ProfileNode::GetSelfTime) {
  NanScope();
#if (NODE_MODULE_VERSION > 0x000B)
  NanReturnUndefined();
#else
  Local<Object> self = args.Holder();
  void* ptr = NanGetInternalFieldPointer(self, 0);
  double stime = static_cast<CpuProfileNode*>(ptr)->GetSelfTime();
  NanReturnValue(Number::New(stime));
#endif
}

NAN_GETTER(ProfileNode::GetTotalSamplesCount) {
  NanScope();
#if (NODE_MODULE_VERSION > 0x000B)
  NanReturnUndefined();
#else
  Local<Object> self = args.Holder();
  void* ptr = NanGetInternalFieldPointer(self, 0);
  double samples = static_cast<CpuProfileNode*>(ptr)->GetTotalSamplesCount();
  NanReturnValue(Number::New(samples));
#endif
}

NAN_GETTER(ProfileNode::GetSelfSamplesCount) {
  NanScope();
  Local<Object> self = args.Holder();
  void* ptr = NanGetInternalFieldPointer(self, 0);
#if (NODE_MODULE_VERSION > 0x000B)
  double samples = static_cast<CpuProfileNode*>(ptr)->GetHitCount();
#else
  double samples = static_cast<CpuProfileNode*>(ptr)->GetSelfSamplesCount();
#endif
  NanReturnValue(Number::New(samples));
}

NAN_GETTER(ProfileNode::GetCallUid) {
  NanScope();
  Local<Object> self = args.Holder();
  void* ptr = NanGetInternalFieldPointer(self, 0);
  uint32_t uid = static_cast<CpuProfileNode*>(ptr)->GetCallUid();
  NanReturnValue(Integer::NewFromUnsigned(uid));
}

NAN_GETTER(ProfileNode::GetChildrenCount) {
  NanScope();
  Local<Object> self = args.Holder();
  void* ptr = NanGetInternalFieldPointer(self, 0);
  int32_t count = static_cast<CpuProfileNode*>(ptr)->GetChildrenCount();
  NanReturnValue(Integer::New(count));
}

NAN_METHOD(ProfileNode::GetChild) {
  NanScope();
  if (args.Length() < 1) {
    return NanThrowError("No index specified");
  } else if (!args[0]->IsInt32()) {
    return NanThrowError("Argument must be integer");
  }
  int32_t index = args[0]->Int32Value();
  Handle<Object> self = args.This();
  void* ptr = NanGetInternalFieldPointer(self, 0);
  const CpuProfileNode* node = static_cast<CpuProfileNode*>(ptr)->GetChild(index);
  NanReturnValue(ProfileNode::New(node));
}

Handle<Value> ProfileNode::New(const CpuProfileNode* node) {
  NanScope();
  
  if (node_template_.IsEmpty()) {
    ProfileNode::Initialize();
  }
  
  if(!node) {
    return Undefined();
  }
  else {
    Local<Object> obj = NanPersistentToLocal(node_template_)->NewInstance();
    NanSetInternalFieldPointer(obj, 0, const_cast<CpuProfileNode*>(node));
    return obj;
  }
}
}