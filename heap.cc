#include <v8.h>
#include <v8-profiler.h>
#include <node.h>

using namespace v8;
using namespace node;

static Persistent<ObjectTemplate> snapshot_template;
static Persistent<ObjectTemplate> graph_node_template;
static Persistent<ObjectTemplate> graph_edge_template;
static Persistent<ObjectTemplate> graph_path_template;
static Persistent<ObjectTemplate> graph_diff_template;


static Handle<Value> GetSnapshotUid(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  uint32_t uid = static_cast<HeapSnapshot*>(ptr)->GetUid();
  return scope.Close(Integer::NewFromUnsigned(uid));
}


static Handle<Value> GetSnapshotTitle(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  Handle<String> title = static_cast<HeapSnapshot*>(ptr)->GetTitle();
  return scope.Close(title);
}

static Handle<Value> GetSnapshotRoot(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  const HeapGraphNode* node = static_cast<HeapSnapshot*>(ptr)->GetRoot();
  if(!node) {
    return Undefined();
  }
  else {
    Local<Object> obj = graph_node_template->NewInstance();
    obj->SetPointerInInternalField(0, const_cast<HeapGraphNode*>(node));
    return scope.Close(obj);
  }
}

static Handle<Value> GetSnapshotType(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  HeapSnapshot::Type type = static_cast<HeapSnapshot*>(ptr)->GetType();
  Local<String> t;
  switch(type) {
    case HeapSnapshot::kFull :
    t = String::New("Full");
    break;
    case HeapSnapshot::kAggregated :
    t = String::New("Aggregated");
    break;
    default:
    t = String::New("Unknown");
    break;
  }
  return scope.Close(t);
}

static Handle<Value> SnapshotCompare(const Arguments& args) {
  HandleScope scope;
  if (args.Length() < 1) {
    return ThrowException(Exception::Error(String::New("No index specified")));
  }
  Handle<Object> other = args[0]->ToObject();
  Handle<Object> self = args.This();
  
  void* ptr = self->GetPointerFromInternalField(0);
  
  void* optr = other->GetPointerFromInternalField(0);
  
  const HeapSnapshotsDiff* diff = static_cast<HeapSnapshot*>(ptr)->CompareWith(static_cast<HeapSnapshot*>(optr));
  if(!diff) {
    return Undefined();
  }
  else {
    Local<Object> obj = graph_diff_template->NewInstance();
    obj->SetPointerInInternalField(0, const_cast<HeapSnapshotsDiff*>(diff));
    return scope.Close(obj);
  }
}

static Handle<Value> GetNodeType(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  HeapGraphNode::Type type = static_cast<HeapGraphNode*>(ptr)->GetType();
  Local<String> t;
  switch(type) {
    case HeapGraphNode::kArray :
    t = String::New("Array");
    break;
    case HeapGraphNode::kString :
    t = String::New("String");
    break;
    case HeapGraphNode::kObject :
    t = String::New("Object");
    break;
    case HeapGraphNode::kCode :
    t = String::New("Code");
    break;
    case HeapGraphNode::kClosure :
    t = String::New("Closure");
    break;
    case HeapGraphNode::kRegExp :
    t = String::New("RegExp");
    break;
    case HeapGraphNode::kHeapNumber :
    t = String::New("HeapNumber");
    break;
    default:
    t = String::New("Hidden");
    break;
  }
  return scope.Close(t);
}

static Handle<Value> GetNodeName(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  Handle<String> title = static_cast<HeapGraphNode*>(ptr)->GetName();
  return scope.Close(title);
}

static Handle<Value> GetNodeId(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  uint64_t id = static_cast<HeapGraphNode*>(ptr)->GetId();
  return scope.Close(Integer::NewFromUnsigned(id));
}

static Handle<Value> GetNodeInstancesCount(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  int32_t count = static_cast<HeapGraphNode*>(ptr)->GetInstancesCount();
  return scope.Close(Integer::New(count));
}

static Handle<Value> GetNodeChildrenCount(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  int32_t count = static_cast<HeapGraphNode*>(ptr)->GetChildrenCount();
  return scope.Close(Integer::New(count));
}

static Handle<Value> GetNodeRetainersCount(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  int32_t count = static_cast<HeapGraphNode*>(ptr)->GetRetainersCount();
  return scope.Close(Integer::New(count));
}

static Handle<Value> GetNodeSize(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  int32_t count = static_cast<HeapGraphNode*>(ptr)->GetSelfSize();
  return scope.Close(Integer::New(count));
}

static Handle<Value> GetNodeChild(const Arguments& args) {
  HandleScope scope;
  if (args.Length() < 1) {
    return ThrowException(Exception::Error(String::New("No index specified")));
  } else if (!args[0]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Argument must be integer")));
  }
  int32_t index = args[0]->Int32Value();
  Handle<Object> self = args.This();
  void* ptr = self->GetPointerFromInternalField(0);
  const HeapGraphEdge* edge = static_cast<HeapGraphNode*>(ptr)->GetChild(index);
  if(!edge) {
    return Undefined();
  }
  else {
    Local<Object> obj = graph_edge_template->NewInstance();
    obj->SetPointerInInternalField(0, const_cast<HeapGraphEdge*>(edge));
    return scope.Close(obj);
  }
}

static Handle<Value> GetNodeRetainedSize(const Arguments& args) {
  HandleScope scope;
  bool exact = false;
  if (args.Length() > 0) {
    exact = args[0]->BooleanValue();
  }
  Handle<Object> self = args.This();
  void* ptr = self->GetPointerFromInternalField(0);
  int32_t size = static_cast<HeapGraphNode*>(ptr)->GetRetainedSize(exact);
  return scope.Close(Integer::New(size));
}

static Handle<Value> GetNodeRetainer(const Arguments& args) {
  HandleScope scope;
  if (args.Length() < 1) {
    return ThrowException(Exception::Error(String::New("No index specified")));
  } else if (!args[0]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Argument must be integer")));
  }
  int32_t index = args[0]->Int32Value();
  Handle<Object> self = args.This();
  void* ptr = self->GetPointerFromInternalField(0);
  const HeapGraphEdge* edge = static_cast<HeapGraphNode*>(ptr)->GetRetainer(index);
  if(!edge) {
    return Undefined();
  }
  else {
    Local<Object> obj = graph_edge_template->NewInstance();
    obj->SetPointerInInternalField(0, const_cast<HeapGraphEdge*>(edge));
    return scope.Close(obj);
  }
}

static Handle<Value> GetNodeRetainingPathsCount(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  int32_t count = static_cast<HeapGraphNode*>(ptr)->GetRetainingPathsCount();
  return scope.Close(Integer::New(count));
}

static Handle<Value> GetNodeRetainingPath(const Arguments& args) {
  HandleScope scope;
  if (args.Length() < 1) {
    return ThrowException(Exception::Error(String::New("No index specified")));
  } else if (!args[0]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Argument must be integer")));
  }
  int32_t index = args[0]->Int32Value();
  Handle<Object> self = args.This();
  void* ptr = self->GetPointerFromInternalField(0);
  const HeapGraphPath* path = static_cast<HeapGraphNode*>(ptr)->GetRetainingPath(index);
  if(!path) {
    return Undefined();
  }
  else {
    Local<Object> obj = graph_path_template->NewInstance();
    obj->SetPointerInInternalField(0, const_cast<HeapGraphPath*>(path));
    return scope.Close(obj);
  }
}

static Handle<Value> GetNodeDominator(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  const HeapGraphNode* node = static_cast<HeapGraphNode*>(ptr)->GetDominatorNode();
  if(!node) {
    return Undefined();
  }
  else {
    Local<Object> obj = graph_node_template->NewInstance();
    obj->SetPointerInInternalField(0, const_cast<HeapGraphNode*>(node));
    return scope.Close(obj);
  }
}

static Handle<Value> GetEdgeType(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  HeapGraphEdge::Type type = static_cast<HeapGraphEdge*>(ptr)->GetType();
  Local<String> t;
  switch(type) {
    case HeapGraphEdge::kContextVariable :
    t = String::New("ContextVariable");
    break;
    case HeapGraphEdge::kElement :
    t = String::New("Element");
    break;
    case HeapGraphEdge::kProperty :
    t = String::New("Property");
    break;
    case HeapGraphEdge::kInternal :
    t = String::New("Internal");
    break;
    case HeapGraphEdge::kHidden :
    t = String::New("Hidden");
    break;
    case HeapGraphEdge::kShortcut :
    t = String::New("Shortcut");
    break;
    default:
    t = String::New("Unknown");
    break;
  }
  return scope.Close(t);
}

static Handle<Value> GetEdgeName(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  Handle<Value> title = static_cast<HeapGraphEdge*>(ptr)->GetName();
  return scope.Close(title);
}

static Handle<Value> GetEdgeFrom(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  const HeapGraphNode* node = static_cast<HeapGraphEdge*>(ptr)->GetFromNode();
  if(!node) {
    return Undefined();
  }
  else {
    Local<Object> obj = graph_node_template->NewInstance();
    obj->SetPointerInInternalField(0, const_cast<HeapGraphNode*>(node));
    return scope.Close(obj);
  }
}

static Handle<Value> GetEdgeTo(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  const HeapGraphNode* node = static_cast<HeapGraphEdge*>(ptr)->GetToNode();
  if(!node) {
    return Undefined();
  }
  else {
    Local<Object> obj = graph_node_template->NewInstance();
    obj->SetPointerInInternalField(0, const_cast<HeapGraphNode*>(node));
    return scope.Close(obj);
  }
}

static Handle<Value> GetPathEdgesCount(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  int32_t count = static_cast<HeapGraphPath*>(ptr)->GetEdgesCount();
  return scope.Close(Integer::New(count));
}

static Handle<Value> GetPathEdge(const Arguments& args) {
  HandleScope scope;
  if (args.Length() < 1) {
    return ThrowException(Exception::Error(String::New("No index specified")));
  } else if (!args[0]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Argument must be integer")));
  }
  int32_t index = args[0]->Int32Value();
  Handle<Object> self = args.This();
  void* ptr = self->GetPointerFromInternalField(0);
  const HeapGraphEdge* edge = static_cast<HeapGraphPath*>(ptr)->GetEdge(index);
  if(!edge) {
    return Undefined();
  }
  else {
    Local<Object> obj = graph_edge_template->NewInstance();
    obj->SetPointerInInternalField(0, const_cast<HeapGraphEdge*>(edge));
    return scope.Close(obj);
  }
}

static Handle<Value> GetPathFrom(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  const HeapGraphNode* node = static_cast<HeapGraphPath*>(ptr)->GetFromNode();
  if(!node) {
    return Undefined();
  }
  else {
    Local<Object> obj = graph_node_template->NewInstance();
    obj->SetPointerInInternalField(0, const_cast<HeapGraphNode*>(node));
    return scope.Close(obj);
  }
}

static Handle<Value> GetPathTo(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  const HeapGraphNode* node = static_cast<HeapGraphPath*>(ptr)->GetToNode();
  if(!node) {
    return Undefined();
  }
  else {
    Local<Object> obj = graph_node_template->NewInstance();
    obj->SetPointerInInternalField(0, const_cast<HeapGraphNode*>(node));
    return scope.Close(obj);
  }
}

static Handle<Value> GetDiffAdditions(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  const HeapGraphNode* node = static_cast<HeapSnapshotsDiff*>(ptr)->GetAdditionsRoot();
  if(!node) {
    return Undefined();
  }
  else {
    Local<Object> obj = graph_node_template->NewInstance();
    obj->SetPointerInInternalField(0, const_cast<HeapGraphNode*>(node));
    return scope.Close(obj);
  }
}

static Handle<Value> GetDiffDeletions(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  const HeapGraphNode* node = static_cast<HeapSnapshotsDiff*>(ptr)->GetDeletionsRoot();
  if(!node) {
    return Undefined();
  }
  else {
    Local<Object> obj = graph_node_template->NewInstance();
    obj->SetPointerInInternalField(0, const_cast<HeapGraphNode*>(node));
    return scope.Close(obj);
  }
}

static Handle<Value> TakeSnapshot(const Arguments& args) {
  HandleScope scope;
  Local<String> title = String::New("");
  if (args.Length() > 0) {
    title = args[0]->ToString();
  }
  const HeapSnapshot* snapshot = HeapProfiler::TakeSnapshot(title);
  if(!snapshot) {
    return Undefined();
  }
  else {
    Local<Object> obj = snapshot_template->NewInstance();
    obj->SetPointerInInternalField(0, const_cast<HeapSnapshot*>(snapshot));
    //obj->SetInternalField(0, External::New(const_cast<HeapSnapshot*>(snapshot)));
    return scope.Close(obj);
  }
}

static Handle<Value> GetSnapshot(const Arguments& args) {
  HandleScope scope;
  if (args.Length() < 1) {
    return ThrowException(Exception::Error(String::New("No index specified")));
  } else if (!args[0]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Argument must be integer")));
  }
  int32_t index = args[0]->Int32Value();
  const HeapSnapshot* snapshot = HeapProfiler::GetSnapshot(index);
  if(!snapshot) {
    return Undefined();
  }
  else {
    Local<Object> obj = snapshot_template->NewInstance();
    obj->SetPointerInInternalField(0, const_cast<HeapSnapshot*>(snapshot));
    return scope.Close(obj);
  }
}

static Handle<Value> FindSnapshot(const Arguments& args) {
  HandleScope scope;
  if (args.Length() < 1) {
    return ThrowException(Exception::Error(String::New("No uid specified")));
  }
  uint32_t uid = args[0]->Uint32Value();
  const HeapSnapshot* snapshot = HeapProfiler::FindSnapshot(uid);
  if(!snapshot) {
    return Undefined();
  }
  else {
    Local<Object> obj = snapshot_template->NewInstance();
    obj->SetPointerInInternalField(0, const_cast<HeapSnapshot*>(snapshot));
    return scope.Close(obj);
  }
}

static Handle<Value> GetSnapshotsCount(const Arguments& args) {
  HandleScope scope;
  return scope.Close(Integer::New(HeapProfiler::GetSnapshotsCount()));
}

extern "C" void init(Handle<Object> target) {
  HandleScope scope;

  snapshot_template = Persistent<ObjectTemplate>::New(ObjectTemplate::New());
  snapshot_template->SetInternalFieldCount(1);
  snapshot_template->SetAccessor(String::New("title"), GetSnapshotTitle);
  snapshot_template->SetAccessor(String::New("uid"), GetSnapshotUid);
  snapshot_template->SetAccessor(String::New("root"), GetSnapshotRoot);
  snapshot_template->SetAccessor(String::New("type"), GetSnapshotType);
  snapshot_template->Set(String::New("compareWith"), FunctionTemplate::New(SnapshotCompare));
  
  graph_node_template = Persistent<ObjectTemplate>::New(ObjectTemplate::New());
  graph_node_template->SetInternalFieldCount(1);
  graph_node_template->SetAccessor(String::New("type"), GetNodeType);
  graph_node_template->SetAccessor(String::New("name"), GetNodeName);
  graph_node_template->SetAccessor(String::New("id"), GetNodeId);
  graph_node_template->SetAccessor(String::New("instancesCount"), GetNodeInstancesCount);
  graph_node_template->SetAccessor(String::New("childrenCount"), GetNodeChildrenCount);
  graph_node_template->SetAccessor(String::New("retainersCount"), GetNodeRetainersCount);
  graph_node_template->SetAccessor(String::New("size"), GetNodeSize);
  graph_node_template->SetAccessor(String::New("retainingPathsCount"), GetNodeRetainingPathsCount);
  graph_node_template->SetAccessor(String::New("dominatorNode"), GetNodeDominator);
  graph_node_template->Set(String::New("getChild"), FunctionTemplate::New(GetNodeChild));
  graph_node_template->Set(String::New("retainedSize"), FunctionTemplate::New(GetNodeRetainedSize));
  graph_node_template->Set(String::New("getRetainer"), FunctionTemplate::New(GetNodeRetainer));
  graph_node_template->Set(String::New("getRetainingPath"), FunctionTemplate::New(GetNodeRetainingPath));
  
  graph_edge_template = Persistent<ObjectTemplate>::New(ObjectTemplate::New());
  graph_edge_template->SetInternalFieldCount(1);
  graph_edge_template->SetAccessor(String::New("type"), GetEdgeType);
  graph_edge_template->SetAccessor(String::New("name"), GetEdgeName);
  graph_edge_template->SetAccessor(String::New("from"), GetEdgeFrom);
  graph_edge_template->SetAccessor(String::New("to"), GetEdgeTo);
  
  graph_path_template = Persistent<ObjectTemplate>::New(ObjectTemplate::New());
  graph_path_template->SetInternalFieldCount(1);
  graph_path_template->SetAccessor(String::New("edgesCount"), GetPathEdgesCount);
  graph_path_template->SetAccessor(String::New("from"), GetPathFrom);
  graph_path_template->SetAccessor(String::New("to"), GetPathTo);
  graph_path_template->Set(String::New("getEdge"), FunctionTemplate::New(GetPathEdge));
  
  graph_diff_template = Persistent<ObjectTemplate>::New(ObjectTemplate::New());
  graph_diff_template->SetInternalFieldCount(1);
  graph_diff_template->SetAccessor(String::New("additions"), GetDiffAdditions);
  graph_diff_template->SetAccessor(String::New("deletions"), GetDiffDeletions);
  
  NODE_SET_METHOD(target, "takeSnapshot", TakeSnapshot);
  NODE_SET_METHOD(target, "getSnapshot", GetSnapshot);
  NODE_SET_METHOD(target, "findSnapshot", FindSnapshot);
  NODE_SET_METHOD(target, "snapshotCount", GetSnapshotsCount);
}