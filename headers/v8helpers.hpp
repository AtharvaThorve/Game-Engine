#pragma once

#define V8H_DEBUG 1

#include <iostream>
#include <v8.h>

/**
 * Some functions for convience with v8
 */
namespace v8helpers {

/**
 * Callback function that enables javascript functions to call `print(..)`
 * and have things print to std::cout
 */
void Print(const v8::FunctionCallbackInfo<v8::Value> &args);

/**
 * Helper function for converting v8 parameters to c++ strings.
 */
const char *ToCString(const v8::String::Utf8Value &value);

/**
 * NOTE: This struct is expected to be used when calling the
 * v8helpers::exposeToV8(...) function.
 */
template <typename G, typename S> struct ParamContainer {
  std::string name_;
  G getter_;
  S setter_;

  ParamContainer(std::string name, G getter, S setter) {
    name_ = name;
    getter_ = getter;
    setter_ = setter;
  }
};

/**
 * Helper function for exposing object instances to the given v8 context.
 *
 * IMPORTANT: Notice the third parameter (list_of_params). That object
 * must be of the specific type in the demonstration in GameObject.cpp
 */
template <typename R, typename G, typename S>
v8::Local<v8::Object>
exposeToV8(std::string object_name, R *object_ref,
           std::vector<ParamContainer<G, S>> list_of_params,
           v8::Isolate *isolate, v8::Local<v8::Context> &context,
           std::string context_name = "default") {
#if V8H_DEBUG
  std::cout << "v8helpers adding " << object_name << std::endl;
#endif
  v8::Context::Scope context_scope(context); // Must enter the context
  v8::EscapableHandleScope handle_scope(isolate);
  v8::Local<v8::ObjectTemplate> object_template =
      v8::ObjectTemplate::New(isolate);

  object_template->SetInternalFieldCount(
      1); // Internal fields are those only accessible in C++...in this case,
          // this object (at least when complete)
  for (ParamContainer p : list_of_params) {
    object_template->SetAccessor(
        v8::String::NewFromUtf8(isolate, p.name_.c_str()), p.getter_,
        p.setter_);
  }

  v8::Local<v8::Object> object =
      object_template->NewInstance(context).ToLocalChecked();
  object->SetInternalField(0, v8::External::New(isolate, object_ref));

  context->Global()->Set(v8::String::NewFromUtf8(isolate, object_name.c_str()),
                         object);
  return handle_scope.Escape(object);
}

} // namespace v8helpers
