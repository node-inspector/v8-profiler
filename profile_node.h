

#ifndef NODE_PROFILE_NODE_
#define NODE_PROFILE_NODE_

#include <node.h>
#include <v8-profiler.h>
#include "nan.h"

using namespace v8;

namespace nodex {

class ProfileNode {
 public:
   static Handle<Value> New(const CpuProfileNode* node);

 private:
   static NAN_GETTER(GetFunctionName);
   static NAN_GETTER(GetScriptName);
   static NAN_GETTER(GetLineNumber);
   static NAN_GETTER(GetTotalTime);
   static NAN_GETTER(GetSelfTime);
   static NAN_GETTER(GetTotalSamplesCount);
   static NAN_GETTER(GetSelfSamplesCount);
   static NAN_GETTER(GetCallUid);
   static NAN_GETTER(GetChildrenCount);
   static NAN_METHOD(GetChild);
   static void Initialize();
   static Persistent<ObjectTemplate> node_template_;
};

}
#endif  // NODE_PROFILE_NODE_
