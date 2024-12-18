#include "v8helpers.hpp"

namespace v8helpers {

/**
 * Extracts a C string from a V8 Utf8Value.
 * Hat tip: https://github.com/v8/v8/blob/master/samples/shell.cc
 */
const char *ToCString(const v8::String::Utf8Value &value) {
  return *value ? *value : "<string conversion failed>";
}

/**
 * The callback that is invoked by v8 whenever the JavaScript 'print'
 * function is called.  Prints its arguments on stdout separated by spaces
 * and ending with a newline.
 * Hat tip: * https://github.com/v8/v8/blob/master/samples/shell.cc
 */
void Print(const v8::FunctionCallbackInfo<v8::Value> &args) {
  bool first = true;
  for (int i = 0; i < args.Length(); i++) {
    // v8::HandleScope handle_scope(args.GetIsolate());
    if (first) {
      first = false;
    } else {
      printf(" ");
    }
    v8::String::Utf8Value str(args.GetIsolate(), args[i]);
    const char *cstr = ToCString(str);
    printf("%s", cstr);
  }
  printf("\n");
  fflush(stdout);
}

} // namespace v8helpers
