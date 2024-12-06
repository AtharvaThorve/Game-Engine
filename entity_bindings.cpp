#include "entity_bindings.hpp"

void get_position(v8::Local<v8::String> property,
                  const v8::PropertyCallbackInfo<v8::Value> &info) {
  auto entity = static_cast<Entity *>(
      info.Holder()->GetInternalField(0).As<v8::External>()->Value());

  Vector2 position = entity->getPosition();

  v8::Local<v8::Object> js_pos = v8::Object::New(info.GetIsolate());

  v8::MaybeLocal<v8::String> x_str =
      v8::String::NewFromUtf8(info.GetIsolate(), "x");
  v8::MaybeLocal<v8::String> y_str =
      v8::String::NewFromUtf8(info.GetIsolate(), "y");

  if (!x_str.IsEmpty() && !y_str.IsEmpty()) {
    js_pos
        ->Set(info.GetIsolate()->GetCurrentContext(), x_str.ToLocalChecked(),
              v8::Number::New(info.GetIsolate(), position.x))
        .Check();
    js_pos
        ->Set(info.GetIsolate()->GetCurrentContext(), y_str.ToLocalChecked(),
              v8::Number::New(info.GetIsolate(), position.y))
        .Check();
  }

  info.GetReturnValue().Set(js_pos);
}

void set_position(v8::Local<v8::String> property, v8::Local<v8::Value> value,
                  const v8::PropertyCallbackInfo<void> &info) {
  if (!value->IsObject()) {
    return;
  }

  auto entity = static_cast<Entity *>(
      info.Holder()->GetInternalField(0).As<v8::External>()->Value());

  v8::Local<v8::Object> js_pos = value.As<v8::Object>();

  v8::MaybeLocal<v8::Value> jsx =
      js_pos
          ->Get(info.GetIsolate()->GetCurrentContext(),
                v8::String::NewFromUtf8(info.GetIsolate(), "x"))
          .ToLocalChecked();
  v8::MaybeLocal<v8::Value> jsy =
      js_pos
          ->Get(info.GetIsolate()->GetCurrentContext(),
                v8::String::NewFromUtf8(info.GetIsolate(), "y"))
          .ToLocalChecked();

  if (jsx.IsEmpty() || jsy.IsEmpty()) {
    return;
  }

  if (jsx.ToLocalChecked()->IsNumber() && jsy.ToLocalChecked()->IsNumber()) {
    Vector2 newPosition{jsx.ToLocalChecked().As<v8::Number>()->Value(),
                        jsy.ToLocalChecked().As<v8::Number>()->Value()};
    entity->setPosition(newPosition);
  }
}

v8::Local<v8::Object> expose_entity_to_v8(std::shared_ptr<Entity> entity,
                                          v8::Isolate *isolate,
                                          v8::Local<v8::Context> &context) {
  v8::EscapableHandleScope handle_scope(isolate);

  std::vector<v8helpers::ParamContainer<v8::AccessorGetterCallback,
                                        v8::AccessorSetterCallback>>
      params;

  params.push_back(
      v8helpers::ParamContainer("position", get_position, set_position));

  return v8helpers::exposeToV8("player", entity.get(), params, isolate,
                               context);
}