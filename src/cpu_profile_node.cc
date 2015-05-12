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
  Handle<Value> ProfileNode::GetLineTicks_(const CpuProfileNode* node) {
    NanEscapableScope();

    uint32_t count = node->GetHitLineCount();
    v8::CpuProfileNode::LineTick *entries = new v8::CpuProfileNode::LineTick[count];
    bool result = node->GetLineTicks(entries, count);

    Local<Value> lineTicks;
    if (result) {
      Local<Array> array = NanNew<Array>(count);
      for (uint32_t index = 0; index < count; index++) {
        Local<Object> tick = NanNew<Object>();
        tick->Set(NanNew<String>("line"),     NanNew<Integer>(entries[index].line));
        tick->Set(NanNew<String>("hitCount"), NanNew<Integer>(entries[index].hit_count));
        array->Set(index, tick);
      }
      lineTicks = array;
    } else {
      lineTicks = NanNull();
    }

    delete[] entries;
    return NanEscapeScope(lineTicks);
  }
#endif
  
  Handle<Value> ProfileNode::New (const CpuProfileNode* node) {
    NanEscapableScope();
    
    int32_t count = node->GetChildrenCount();
    Local<Object> profile_node = NanNew<Object>();
    Local<Array> children = NanNew<Array>(count);
    
    for (int32_t index = 0; index < count; index++) {
      children->Set(index, ProfileNode::New(node->GetChild(index)));
    }
    
    profile_node->Set(NanNew<String>("functionName"),  NanNew<String>(node->GetFunctionName()));
    profile_node->Set(NanNew<String>("url"),           NanNew<String>(node->GetScriptResourceName()));
    profile_node->Set(NanNew<String>("lineNumber"),    NanNew<Integer>(node->GetLineNumber()));
    profile_node->Set(NanNew<String>("callUID"),       NanNew<Number>(node->GetCallUid()));
#if (NODE_MODULE_VERSION > 0x000B)      
    profile_node->Set(NanNew<String>("bailoutReason"), NanNew<String>(node->GetBailoutReason()));
    profile_node->Set(NanNew<String>("id"),            NanNew<Integer>(node->GetNodeId()));
    profile_node->Set(NanNew<String>("scriptId"),      NanNew<Integer>(node->GetScriptId()));
    profile_node->Set(NanNew<String>("hitCount"),      NanNew<Integer>(node->GetHitCount()));
#else
    profile_node->Set(NanNew<String>("bailoutReason"), NanNew<String>("no reason"));
    profile_node->Set(NanNew<String>("id"),            NanNew<Integer>(UIDCounter++));
    //TODO(3y3): profile_node->Set(NanNew<String>("scriptId"),      NanNew<Integer>(node->GetScriptId()));
    profile_node->Set(NanNew<String>("hitCount"),      NanNew(node->GetSelfSamplesCount()));
#endif
    profile_node->Set(NanNew<String>("children"),      children);

#if (NODE_MODULE_VERSION >= 42)
    Handle<Value> lineTicks = GetLineTicks_(node);
    if (!lineTicks->IsNull()) {
      profile_node->Set(NanNew<String>("lineTicks"), lineTicks);
    }
#endif

    return NanEscapeScope(profile_node);
  }

}
