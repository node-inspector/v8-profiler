#include "snapshot_diff.h"
#include "graph_node.h"

using namespace v8;

namespace nodex {
  
Persistent<ObjectTemplate> SnapshotDiff::diff_template_;
  
void SnapshotDiff::Initialize() {
  diff_template_ = Persistent<ObjectTemplate>::New(ObjectTemplate::New());
  diff_template_->SetInternalFieldCount(1);
  diff_template_->SetAccessor(String::New("additions"), SnapshotDiff::GetAdditions);
  diff_template_->SetAccessor(String::New("deletions"), SnapshotDiff::GetDeletions);
}

Handle<Value> SnapshotDiff::GetAdditions(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  const HeapGraphNode* node = static_cast<HeapSnapshotsDiff*>(ptr)->GetAdditionsRoot();
  return scope.Close(GraphNode::New(node));
}

Handle<Value> SnapshotDiff::GetDeletions(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  const HeapGraphNode* node = static_cast<HeapSnapshotsDiff*>(ptr)->GetDeletionsRoot();
  return scope.Close(GraphNode::New(node));
}

Handle<Value> SnapshotDiff::New(const HeapSnapshotsDiff* diff) {
  HandleScope scope;
  
  if (diff_template_.IsEmpty()) {
    SnapshotDiff::Initialize();
  }
  
  if(!diff) {
    return Undefined();
  }
  else {
    Local<Object> obj = diff_template_->NewInstance();
    obj->SetPointerInInternalField(0, const_cast<HeapSnapshotsDiff*>(diff));
    return scope.Close(obj);
  }
}

}