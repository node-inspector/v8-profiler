#include "profile.h"
#include "profile_node.h"

using namespace v8;

namespace nodex {

Persistent<ObjectTemplate> Profile::profile_template_;

void Profile::Initialize() {
  Local<ObjectTemplate> tpl = NanNewLocal<ObjectTemplate>(ObjectTemplate::New());
  NanAssignPersistent(ObjectTemplate, profile_template_, tpl);
  tpl->SetInternalFieldCount(1);
  tpl->SetAccessor(String::New("title"), Profile::GetTitle);
  tpl->SetAccessor(String::New("uid"), Profile::GetUid);
  tpl->SetAccessor(String::New("topRoot"), Profile::GetTopRoot);
  tpl->SetAccessor(String::New("bottomRoot"), Profile::GetBottomRoot);
  tpl->Set(String::New("delete"), FunctionTemplate::New(Profile::Delete));
}

NAN_GETTER(Profile::GetUid) {
  NanScope();
  Local<Object> self = args.Holder();
  void* ptr = NanGetInternalFieldPointer(self, 0);

  uint32_t uid = static_cast<CpuProfile*>(ptr)->GetUid();
  NanReturnValue(Integer::NewFromUnsigned(uid));
}


NAN_GETTER(Profile::GetTitle) {
  NanScope();
  Local<Object> self = args.Holder();
  void* ptr = NanGetInternalFieldPointer(self, 0);
  Handle<String> title = static_cast<CpuProfile*>(ptr)->GetTitle();
  NanReturnValue(title);
}

NAN_GETTER(Profile::GetTopRoot) {
  NanScope();
  Local<Object> self = args.Holder();
  void* ptr = NanGetInternalFieldPointer(self, 0);
  const CpuProfileNode* node = static_cast<CpuProfile*>(ptr)->GetTopDownRoot();
  NanReturnValue(ProfileNode::New(node));
}


NAN_GETTER(Profile::GetBottomRoot) {
  NanScope();
#if (NODE_MODULE_VERSION > 0x000B)
  NanReturnUndefined();
#else
  Local<Object> self = args.Holder();
  void* ptr = NanGetInternalFieldPointer(self, 0);
  const CpuProfileNode* node = static_cast<CpuProfile*>(ptr)->GetBottomUpRoot();
  NanReturnValue(ProfileNode::New(node));
#endif
}

NAN_METHOD(Profile::Delete) {
  NanScope();
  Handle<Object> self = args.This();
  void* ptr = NanGetInternalFieldPointer(self, 0);
  static_cast<CpuProfile*>(ptr)->Delete();
  NanReturnUndefined();
}

Handle<Value> Profile::New(const CpuProfile* profile) {
  NanScope();
  
  if (profile_template_.IsEmpty()) {
    Profile::Initialize();
  }
  
  if(!profile) {
    return Undefined();
  }
  else {
    Local<Object> obj = NanPersistentToLocal(profile_template_)->NewInstance();
    NanSetInternalFieldPointer(obj, 0, const_cast<CpuProfile*>(profile));
    return obj;
  }
}
}
