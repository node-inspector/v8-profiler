#include "cpu_profile_node.h"

namespace nodex {
  using v8::CpuProfileNode;
  using v8::Handle;
  using v8::String;
  using v8::Number;
  using v8::Integer;
  using v8::Value;
  using v8::Local;
  using v8::Object;
  using v8::Array;

  uint32_t ProfileNode::UIDCounter = 1;

#if (NODE_MODULE_VERSION >= 42)
  Local<Value> ProfileNode::GetLineTicks_(const CpuProfileNode* node) {
    Nan::EscapableHandleScope scope;

    uint32_t count = node->GetHitLineCount();
    v8::CpuProfileNode::LineTick *entries = new v8::CpuProfileNode::LineTick[count];
    bool result = node->GetLineTicks(entries, count);

    Local<Value> lineTicks;
    if (result) {
      Local<Array> array = Nan::New<Array>(count);
      for (uint32_t index = 0; index < count; index++) {
        Local<Object> tick = Nan::New<Object>();
        tick->Set(Nan::New<String>("line").ToLocalChecked(),     Nan::New<Integer>(entries[index].line));
        tick->Set(Nan::New<String>("hitCount").ToLocalChecked(), Nan::New<Integer>(entries[index].hit_count));
        array->Set(index, tick);
      }
      lineTicks = array;
    } else {
      lineTicks = Nan::Null();
    }

    delete[] entries;
    return scope.Escape(lineTicks);
  }
#endif

  Local<Value> ProfileNode::New (const CpuProfileNode* node) {
    Nan::EscapableHandleScope scope;

    int32_t count = node->GetChildrenCount();
    Local<Object> profile_node = Nan::New<Object>();
    Local<Array> children = Nan::New<Array>(count);

    for (int32_t index = 0; index < count; index++) {
      children->Set(index, ProfileNode::New(node->GetChild(index)));
    }

    profile_node->Set(Nan::New<String>("functionName").ToLocalChecked(),  node->GetFunctionName());
    profile_node->Set(Nan::New<String>("url").ToLocalChecked(),           node->GetScriptResourceName());
    profile_node->Set(Nan::New<String>("lineNumber").ToLocalChecked(),    Nan::New<Integer>(node->GetLineNumber()));
    profile_node->Set(Nan::New<String>("callUID").ToLocalChecked(),       Nan::New<Number>(node->GetCallUid()));
#if (NODE_MODULE_VERSION > 0x000B)
    profile_node->Set(Nan::New<String>("bailoutReason").ToLocalChecked(), Nan::New<String>(node->GetBailoutReason()).ToLocalChecked());
    profile_node->Set(Nan::New<String>("id").ToLocalChecked(),            Nan::New<Integer>(node->GetNodeId()));
    profile_node->Set(Nan::New<String>("scriptId").ToLocalChecked(),      Nan::New<Integer>(node->GetScriptId()));
    profile_node->Set(Nan::New<String>("hitCount").ToLocalChecked(),      Nan::New<Integer>(node->GetHitCount()));
#else
    profile_node->Set(Nan::New<String>("bailoutReason").ToLocalChecked(), Nan::New<String>("no reason").ToLocalChecked());
    profile_node->Set(Nan::New<String>("id").ToLocalChecked(),            Nan::New<Integer>(UIDCounter++));
    //TODO(3y3): profile_node->Set(Nan::New<String>("scriptId").ToLocalChecked(),      Nan::New<Integer>(node->GetScriptId()));
    profile_node->Set(Nan::New<String>("hitCount").ToLocalChecked(),      Nan::New<Integer>(static_cast<uint32_t>(node->GetSelfSamplesCount())));
#endif
    profile_node->Set(Nan::New<String>("children").ToLocalChecked(),      children);

#if (NODE_MODULE_VERSION >= 42)
    Local<Value> lineTicks = GetLineTicks_(node);
    if (!lineTicks->IsNull()) {
      profile_node->Set(Nan::New<String>("lineTicks").ToLocalChecked(), lineTicks);
    }
#endif

    return scope.Escape(profile_node);
  }

}
