#ifndef NODE_PROFILE_
#define NODE_PROFILE_

#include <node.h>
#include <v8-profiler.h>

using namespace v8;

namespace nodex {

class Profile {
 public:
  static Handle<Value> New(const CpuProfile* profile);
 
 private:
  static Handle<Value> GetUid(Local<String> property, const AccessorInfo& info);
  static Handle<Value> GetTitle(Local<String> property, const AccessorInfo& info);
  static Handle<Value> GetTopRoot(Local<String> property, const AccessorInfo& info);
  static Handle<Value> GetBottomRoot(Local<String> property, const AccessorInfo& info);
  static Handle<Value> Delete(const Arguments& args);
  static void Initialize();
  static Persistent<ObjectTemplate> profile_template_;
};

} //namespace nodex
#endif  // NODE_PROFILE_
