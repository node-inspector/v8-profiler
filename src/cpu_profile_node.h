#ifndef NODE_PROFILE_NODE_
#define NODE_PROFILE_NODE_

#include "v8-profiler.h"
#include "nan.h"

namespace nodex {

class ProfileNode {
  public:
    static v8::Local<v8::Value> New(const v8::CpuProfileNode* node);
    static uint32_t UIDCounter;

  private:
#if (NODE_MODULE_VERSION >= 42)
    static v8::Local<v8::Value> GetLineTicks_(const v8::CpuProfileNode* node);
#endif
};

}
#endif  // NODE_PROFILE_NODE_
