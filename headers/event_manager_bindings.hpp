#pragma once
#include "entity.hpp"
#include "event_manager.hpp"
#include "timeline.hpp"
#include <v8.h>

void process_events(const v8::FunctionCallbackInfo<v8::Value> &args);
void raise_event(const v8::FunctionCallbackInfo<v8::Value> &args);
void bind_event_manager(v8::Isolate *isolate,
                        v8::Local<v8::ObjectTemplate> global);
