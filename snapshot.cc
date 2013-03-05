#include "snapshot.h"
#include "node.h"
#include "node_buffer.h"

using namespace v8;
using namespace node;

namespace nodex {

Persistent<ObjectTemplate> Snapshot::snapshot_template_;

void Snapshot::Initialize() {
  snapshot_template_ = Persistent<ObjectTemplate>::New(ObjectTemplate::New());
  snapshot_template_->SetInternalFieldCount(1);
  snapshot_template_->SetAccessor(String::New("title"), Snapshot::GetTitle);
  snapshot_template_->SetAccessor(String::New("uid"), Snapshot::GetUid);
  snapshot_template_->SetAccessor(String::New("type"), Snapshot::GetType);
  snapshot_template_->Set(String::New("delete"), FunctionTemplate::New(Snapshot::Delete));
  snapshot_template_->Set(String::New("serialize"), FunctionTemplate::New(Snapshot::Serialize));
}

Handle<Value> Snapshot::GetTitle(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  Handle<String> title = static_cast<HeapSnapshot*>(ptr)->GetTitle();
  return scope.Close(title);
}

Handle<Value> Snapshot::GetUid(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);
  uint32_t uid = static_cast<HeapSnapshot*>(ptr)->GetUid();
  return scope.Close(Integer::NewFromUnsigned(uid));
}

Handle<Value> Snapshot::GetType(Local<String> property, const AccessorInfo& info) {
  HandleScope scope;
  Local<Object> self = info.Holder();
  void* ptr = self->GetPointerFromInternalField(0);

  HeapSnapshot::Type type = static_cast<HeapSnapshot*>(ptr)->GetType();
  Local<String> t;

  switch(type) {
    case HeapSnapshot::kFull:
      t = String::New("Full");
      break;
    default:
      t = String::New("Unknown");
  }

  return scope.Close(t);
}

Handle<Value> Snapshot::Delete(const Arguments& args) {
    HandleScope scope;
    Handle<Object> self = args.This();
    void* ptr = self->GetPointerFromInternalField(0);
    static_cast<HeapSnapshot*>(ptr)->Delete();
    return Undefined();
}

Handle<Value> Snapshot::New(const HeapSnapshot* snapshot) {
  HandleScope scope;

  if (snapshot_template_.IsEmpty()) {
    Snapshot::Initialize();
  }

  if(!snapshot) {
    return Undefined();
  }
  else {
    Local<Object> obj = snapshot_template_->NewInstance();
    obj->SetPointerInInternalField(0, const_cast<HeapSnapshot*>(snapshot));
    return scope.Close(obj);
  }
}

class OutputStreamAdapter : public v8::OutputStream {
  public:
    OutputStreamAdapter(Handle<Value> arg) {
      Local<String> onEnd = String::New("onEnd");
      Local<String> onData = String::New("onData");

      if (!arg->IsObject()) {
        ThrowException(Exception::TypeError(
          String::New("You must specify an Object as first argument")));
      }

      obj = arg->ToObject();
      if (!obj->Has(onEnd) || !obj->Has(onData)) {
        ThrowException(Exception::TypeError(
          String::New("You must specify properties 'onData' and 'onEnd' to invoke this function")));
      }

      if (!obj->Get(onEnd)->IsFunction() || !obj->Get(onData)->IsFunction()) {
        ThrowException(Exception::TypeError(
          String::New("Properties 'onData' and 'onEnd' have to be functions")));
      }

      onEndFunction = Local<Function>::Cast(obj->Get(onEnd));
      onDataFunction = Local<Function>::Cast(obj->Get(onData));

      abort = Local<Value>::New(Boolean::New(false));
    }

    void EndOfStream() {
      TryCatch try_catch;
      onEndFunction->Call(obj, 0, NULL);

      if (try_catch.HasCaught()) {
        FatalException(try_catch);
      }
    }

    int GetChunkSize() {
      return 10240;
    }

    WriteResult WriteAsciiChunk(char* data, int size) {
      HandleScope scope;

      Handle<Value> argv[2] = {
        Buffer::New(data, size)->handle_,
        Integer::New(size)
      };

      TryCatch try_catch;
      abort = onDataFunction->Call(obj, 2, argv);

      if (try_catch.HasCaught()) {
        FatalException(try_catch);
        return kAbort;
      }

      if (abort.IsEmpty() || !abort->IsBoolean()) {
        return kContinue;
      }

      return abort->IsTrue() ? kAbort : kContinue;
    }

  private:
    Local<Value> abort;
    Handle<Object> obj;
    Handle<Function> onEndFunction;
    Handle<Function> onDataFunction;
};

Handle<Value> Snapshot::Serialize(const Arguments& args) {
  HandleScope scope;
  Handle<Object> self = args.This();

  uint32_t argslen = args.Length();

  if (argslen == 0) {
    return ThrowException(Exception::TypeError(
      String::New("You must specify arguments to invoke this function")));
  }

  OutputStreamAdapter *stream = new OutputStreamAdapter(args[0]);

  void* ptr = self->GetPointerFromInternalField(0);
  static_cast<HeapSnapshot*>(ptr)->Serialize(stream, HeapSnapshot::kJSON);

  return Undefined();
}

} //namespace nodex
