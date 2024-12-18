#include "event_manager_bindings.hpp"

extern Timeline globalTimeline;

void process_events(const v8::FunctionCallbackInfo<v8::Value> &args) {
  v8::Isolate *isolate = args.GetIsolate();
  v8::HandleScope handle_scope(isolate);

  int64_t timestamp = globalTimeline.getTime();
  EventManager::getInstance().process_events(timestamp);
}

void raise_event(const v8::FunctionCallbackInfo<v8::Value> &args) {
  v8::Isolate *isolate = args.GetIsolate();
  v8::HandleScope handle_scope(isolate);

  if (args.Length() < 1 || !args[0]->IsString()) {
    isolate->ThrowException(v8::String::NewFromUtf8(
        isolate, "Invalid arguments. Expected event type as string."));
    return;
  }

  v8::String::Utf8Value event_type(isolate, args[0]);

  int64_t timestamp = globalTimeline.getTime() + 1;

  Event event(*event_type, timestamp);

  if (args.Length() > 1 && args[1]->IsObject()) {
    v8::Local<v8::Object> js_params =
        args[1]->ToObject(isolate->GetCurrentContext()).ToLocalChecked();
    v8::Local<v8::Array> keys =
        js_params->GetPropertyNames(isolate->GetCurrentContext())
            .ToLocalChecked();

    for (uint32_t i = 0; i < keys->Length(); ++i) {
      v8::Local<v8::Value> key =
          keys->Get(isolate->GetCurrentContext(), i).ToLocalChecked();
      v8::Local<v8::Value> value =
          js_params->Get(isolate->GetCurrentContext(), key).ToLocalChecked();

      v8::String::Utf8Value param_name(isolate, key);
      std::string key_str(*param_name);
      if (value->IsString()) {
        v8::String::Utf8Value param_value(isolate, value);
        event.parameters[key_str] = std::string(*param_value);
      } else if (value->IsInt32()) {
        event.parameters[key_str] =
            value->Int32Value(isolate->GetCurrentContext()).ToChecked();
      } else if (value->IsNumber()) {
        event.parameters[key_str] = static_cast<float>(
            value->NumberValue(isolate->GetCurrentContext()).ToChecked());
      } else if (value->IsObject()) {
        v8::Local<v8::Object> obj =
            value->ToObject(isolate->GetCurrentContext()).ToLocalChecked();

        // Check if it is a Vector2
        if (obj->Has(isolate->GetCurrentContext(),
                     v8::String::NewFromUtf8(isolate, "x"))
                .ToChecked() &&
            obj->Has(isolate->GetCurrentContext(),
                     v8::String::NewFromUtf8(isolate, "y"))
                .ToChecked()) {
          float x = obj->Get(isolate->GetCurrentContext(),
                             v8::String::NewFromUtf8(isolate, "x"))
                        .ToLocalChecked()
                        ->NumberValue(isolate->GetCurrentContext())
                        .ToChecked();
          float y = obj->Get(isolate->GetCurrentContext(),
                             v8::String::NewFromUtf8(isolate, "y"))
                        .ToLocalChecked()
                        ->NumberValue(isolate->GetCurrentContext())
                        .ToChecked();
          event.parameters[key_str] = Vector2{x, y};
        } else if (obj->InternalFieldCount() > 0) {
          v8::Local<v8::External> entity_ptr =
              v8::Local<v8::External>::Cast(obj->GetInternalField(0));
          if (!entity_ptr.IsEmpty()) {
            Entity *raw_entity_ptr = static_cast<Entity *>(entity_ptr->Value());

            std::shared_ptr<Entity> entity_shared_ptr(raw_entity_ptr,
                                                      [](Entity *) {
                                                        // No-op deleter: do
                                                        // nothing
                                                      });

            event.parameters[key_str] = entity_shared_ptr;
          } else {
            std::cerr << "Failed to retrieve Entity pointer for key: "
                      << key_str << std::endl;
          }
        } else {
          std::cerr << "Unhandled object type for key: " << key_str
                    << std::endl;
        }
      } else {
        std::cerr << "Unhandled parameter type for key: " << key_str
                  << std::endl;
      }
    }
  }
  EventManager::getInstance().raise_event(event);
}

void bind_event_manager(v8::Isolate *isolate,
                        v8::Local<v8::ObjectTemplate> global) {
  global->Set(isolate, "process_events",
              v8::FunctionTemplate::New(isolate, process_events));
  global->Set(isolate, "raise_event",
              v8::FunctionTemplate::New(isolate, raise_event));
}