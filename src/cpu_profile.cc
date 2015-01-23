#include "cpu_profile.h"
#include "cpu_profile_node.h"

namespace nodex {
  using v8::Array;
  using v8::CpuProfile;
  using v8::CpuProfileNode;
  using v8::Handle;
  using v8::Number;
  using v8::Integer;
  using v8::Local;
  using v8::Object;
  using v8::ObjectTemplate;
  using v8::FunctionTemplate;
  using v8::Persistent;
  using v8::String;
  using v8::Function;
  using v8::Value;

  Persistent<ObjectTemplate> Profile::profile_template_;
  Persistent<Array> Profile::profiles;
  uint32_t Profile::uid_counter = 0;
  
  void Profile::Initialize () {
    NanScope();
    
    Local<FunctionTemplate> f = NanNew<FunctionTemplate>();
    Local<ObjectTemplate> o = f->InstanceTemplate();
    o->SetInternalFieldCount(1);
    NODE_SET_METHOD(o, "delete", Profile::Delete);
    NanAssignPersistent(profile_template_, o);
  }

  NAN_METHOD(Profile::Delete) {
    NanScope();
    
    Handle<Object> self = args.This();
    void* ptr = NanGetInternalFieldPointer(self, 0);
    Local<Array> profiles = NanNew<Array>(Profile::profiles);
    
    uint32_t count = profiles->Length();
    for (uint32_t index = 0; index < count; index++) {
      if (profiles->Get(index) == args.This()) {
        Local<Value> argv[2] = {
          NanNew<Integer>(index),
          NanNew<Integer>(1)
        };
        Handle<Function>::Cast(profiles->Get(NanNew<String>("splice")))->Call(profiles, 2, argv);
        break;
      }
    }
    static_cast<CpuProfile*>(ptr)->Delete();

    NanReturnUndefined();
  }

  Handle<Value> Profile::New (const CpuProfile* node) {
    NanEscapableScope();
    
    if (profile_template_.IsEmpty()) {
      Profile::Initialize();
    }
    
    uid_counter++;
      
    Local<Object> profile = NanNew(profile_template_)->NewInstance();
    NanSetInternalFieldPointer(profile, 0, const_cast<CpuProfile*>(node));
    
    Local<Value> CPU = NanNew<String>("CPU");
    Local<Value> uid = NanNew<Integer>(uid_counter);
    Handle<String> title = node->GetTitle();
    if (!title->Length()) {
      char _title[32];
      sprintf(_title, "Profile %i", uid_counter);
      title = NanNew<String>(_title);
    }
    Handle<Value> head = ProfileNode::New(node->GetTopDownRoot());
    
    profile->Set(NanNew<String>("typeId"),    CPU);
    profile->Set(NanNew<String>("uid"),       uid);
    profile->Set(NanNew<String>("title"),     title);
    profile->Set(NanNew<String>("head"),      head);

#if (NODE_MODULE_VERSION > 0x000B)
    Local<Value> start_time = NanNew<Number>(node->GetStartTime()/1000000);
    Local<Value> end_time = NanNew<Number>(node->GetEndTime()/1000000);
    Local<Array> samples = NanNew<Array>();
    
    uint32_t count = node->GetSamplesCount();
    for (uint32_t index = 0; index < count; ++index) {
      samples->Set(index, NanNew<Integer>(node->GetSample(index)->GetNodeId()));
    }
    
    profile->Set(NanNew<String>("startTime"), start_time);
    profile->Set(NanNew<String>("endTime"),   end_time);
    profile->Set(NanNew<String>("samples"),   samples);
#endif
    
    Local<Array> profiles = NanNew<Array>(Profile::profiles);
    profiles->Set(profiles->Length(), profile);
    
    return NanEscapeScope(profile);
  }
}
