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
  using v8::String;
  using v8::Function;
  using v8::Value;

  Nan::Persistent<ObjectTemplate> Profile::profile_template_;
  Nan::Persistent<Array> Profile::profiles;
  uint32_t Profile::uid_counter = 0;

  NAN_METHOD(Profile_EmptyMethod) {
  }

  void Profile::Initialize () {
    Nan::HandleScope scope;

    Local<FunctionTemplate> f = Nan::New<FunctionTemplate>(Profile_EmptyMethod);
    Local<ObjectTemplate> o = f->InstanceTemplate();
    o->SetInternalFieldCount(1);
    Nan::SetMethod(o, "delete", Profile::Delete);
    profile_template_.Reset(o);
  }

  NAN_METHOD(Profile::Delete) {
    Local<Object> self = info.This();
    void* ptr = Nan::GetInternalFieldPointer(self, 0);
    Local<Array> profiles = Nan::New<Array>(Profile::profiles);

    uint32_t count = profiles->Length();
    for (uint32_t index = 0; index < count; index++) {
      if (profiles->Get(index) == info.This()) {
        Local<Value> argv[2] = {
          Nan::New<Integer>(index),
          Nan::New<Integer>(1)
        };
        Local<Function>::Cast(profiles->Get(Nan::New<String>("splice").ToLocalChecked()))->Call(profiles, 2, argv);
        break;
      }
    }
    static_cast<CpuProfile*>(ptr)->Delete();
  }

  Local<Value> Profile::New (const CpuProfile* node) {
    Nan::EscapableHandleScope scope;

    if (profile_template_.IsEmpty()) {
      Profile::Initialize();
    }

    uid_counter++;

    Local<Object> profile = Nan::New(profile_template_)->NewInstance();
    Nan::SetInternalFieldPointer(profile, 0, const_cast<CpuProfile*>(node));

    Local<Value> CPU = Nan::New<String>("CPU").ToLocalChecked();
    Local<Value> uid = Nan::New<Integer>(uid_counter);
#if (NODE_MODULE_VERSION >= 45)
    Local<String> title = node->GetTitle();
#else
    Local<String> title = Nan::New(node->GetTitle());
#endif
    if (!title->Length()) {
      char _title[32];
      sprintf(_title, "Profile %i", uid_counter);
      title = Nan::New<String>(_title).ToLocalChecked();
    }
    Local<Value> head = ProfileNode::New(node->GetTopDownRoot());

    profile->Set(Nan::New<String>("typeId").ToLocalChecked(),    CPU);
    profile->Set(Nan::New<String>("uid").ToLocalChecked(),       uid);
    profile->Set(Nan::New<String>("title").ToLocalChecked(),     title);
    profile->Set(Nan::New<String>("head").ToLocalChecked(),      head);

#if (NODE_MODULE_VERSION > 0x000B)
    Local<Value> start_time = Nan::New<Number>(node->GetStartTime()/1000000);
    Local<Value> end_time = Nan::New<Number>(node->GetEndTime()/1000000);
    Local<Array> samples = Nan::New<Array>();

    uint32_t count = node->GetSamplesCount();
    for (uint32_t index = 0; index < count; ++index) {
      samples->Set(index, Nan::New<Integer>(node->GetSample(index)->GetNodeId()));
    }

    profile->Set(Nan::New<String>("startTime").ToLocalChecked(), start_time);
    profile->Set(Nan::New<String>("endTime").ToLocalChecked(),   end_time);
    profile->Set(Nan::New<String>("samples").ToLocalChecked(),   samples);
#endif

    Local<Array> profiles = Nan::New<Array>(Profile::profiles);
    profiles->Set(profiles->Length(), profile);

    return scope.Escape(profile);
  }
}
