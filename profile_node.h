

#ifndef NODE_PROFILE_NODE_
#define NODE_PROFILE_NODE_

#include <v8-profiler.h>

using namespace v8;

namespace nodex {

class ProfileNode {
 public:
   static Handle<Value> New(const CpuProfileNode* node);

 private:
   static Handle<Value> GetFunctionName(Local<String> property, const AccessorInfo& info);
   static Handle<Value> GetScriptName(Local<String> property, const AccessorInfo& info);
   static Handle<Value> GetLineNumber(Local<String> property, const AccessorInfo& info);
   static Handle<Value> GetTotalTime(Local<String> property, const AccessorInfo& info);
   static Handle<Value> GetSelfTime(Local<String> property, const AccessorInfo& info);
   static Handle<Value> GetTotalSamplesCount(Local<String> property, const AccessorInfo& info);
   static Handle<Value> GetSelfSamplesCount(Local<String> property, const AccessorInfo& info);
   static Handle<Value> GetCallUid(Local<String> property, const AccessorInfo& info);
   static Handle<Value> GetChildrenCount(Local<String> property, const AccessorInfo& info);
   static Handle<Value> GetChild(const Arguments& args);
   static void Initialize();
   static Persistent<ObjectTemplate> node_template_;
};

}
#endif  // NODE_PROFILE_NODE_