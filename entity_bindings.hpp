#pragma once
#include "entity.hpp"
#include "v8helpers.hpp"
#include <v8.h>

void get_position(v8::Local<v8::String> property,
                  const v8::PropertyCallbackInfo<v8::Value> &info);

void set_position(v8::Local<v8::String> property, v8::Local<v8::Value> value,
                 const v8::PropertyCallbackInfo<void> &info);

v8::Local<v8::Object> expose_entity_to_v8(std::shared_ptr<Entity> entity, v8::Isolate *isolate,
                      v8::Local<v8::Context> &context);