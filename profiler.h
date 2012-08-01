// I wrote this one from scratch -cat

#ifndef PROFILER_H
#define PROFILER_H

#include <v8-profiler.h>
#include <js/js.h>

using namespace v8;

#define ENTER_ISOLATE 							\
	Locker _l(getIsolate());					\
	HandleScope scope;							\
	Handle<Context> _context = getContext();	\
	Context::Scope _context_scope(_context);	\
	TryCatch _try_catch;

namespace nodex {


  void InitializeProfiler(Handle<ObjectTemplate> target);


} // namespace nodex

#endif // PROFILER_H

