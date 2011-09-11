#include "profile.h"
#include "profile_node.h"

using namespace v8;

namespace nodex {

Persistent<ObjectTemplate> Profile::profile_template_;

void Profile::Initialize() {
  profile_template_ = Persistent<ObjectTemplate>::New(ObjectTemplate::New());
  profile_template_->SetInternalFieldCount(1);
  profile_template_->SetAccessor(String::New("title"), Profile::GetTitle);
  profile_template_->SetAccessor(String::New("uid"), Profile::GetUid);
  profile_template_->SetAccessor(String::New("topRoot"), Profile::GetTopRoot);
  profile_template_->SetAccessor(String::New("bottomRoot"), Profile::GetBottomRoot);
  profile_template_->Set(String::New("delete"), FunctionTemplate::New(Profile::Delete));
}

Handle<Value> Profile::GetUid(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  uint32_t uid = static_cast<CpuProfile*>(ptr)->GetUid();
  return scope.Close(Integer::NewFromUnsigned(uid));
}


Handle<Value> Profile::GetTitle(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  Handle<String> title = static_cast<CpuProfile*>(ptr)->GetTitle();
  return scope.Close(title);
}

Handle<Value> Profile::GetTopRoot(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  const CpuProfileNode* node = static_cast<CpuProfile*>(ptr)->GetTopDownRoot();
  return scope.Close(ProfileNode::New(node));
}


Handle<Value> Profile::GetBottomRoot(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  const CpuProfileNode* node = static_cast<CpuProfile*>(ptr)->GetBottomUpRoot();
  return scope.Close(ProfileNode::New(node));
}

Handle<Value> Profile::Delete(const Arguments& args) {
	HandleScope scope;
  Handle<Object> self = args.This();
	void* ptr = self->GetPointerFromInternalField(0);
  static_cast<CpuProfile*>(ptr)->Delete();
	return Undefined();
}

Handle<Value> Profile::New(const CpuProfile* profile) {
  HandleScope scope;
  
  if (profile_template_.IsEmpty()) {
    Profile::Initialize();
  }
  
  if(!profile) {
    return Undefined();
  }
  else {
    Local<Object> obj = profile_template_->NewInstance();
    obj->SetPointerInInternalField(0, const_cast<CpuProfile*>(profile));
    return scope.Close(obj);
  }
}
}
