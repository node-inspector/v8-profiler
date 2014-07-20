#ifndef NODE_PROFILE_NODE_
#define NODE_PROFILE_NODE_

#include "v8-profiler.h"
#include "nan.h"

namespace nodex {

class ProfileNode {
 public:
   static v8::Handle<v8::Value> New(const v8::CpuProfileNode* node);
   static uint32_t UIDCounter;
};

}
#endif  // NODE_PROFILE_NODE_
