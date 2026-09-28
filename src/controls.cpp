#include "./controls.h"

#include <cmath>

namespace {

using asw::input::ControllerAxis;
using asw::input::ControllerAxisBinding;
using asw::input::ControllerButton;
using asw::input::ControllerButtonBinding;
using asw::input::Key;
using asw::input::KeyBinding;
using asw::input::MouseButton;
using asw::input::MouseButtonBinding;

// asw bindings target one controller index, so bind the first few
constexpr uint32_t MAX_PADS = 4;

constexpr float STICK_DEAD_ZONE = 0.25F;
constexpr float TRIGGER_THRESHOLD = 0.3F;

bool pad_active = false;

void bind_button(const char* action, ControllerButton button) {
  for (uint32_t i = 0; i < MAX_PADS; i++) {
    asw::input::bind_action(action, ControllerButtonBinding{button, i});
  }
}

void bind_axis(const char* action,
               ControllerAxis axis,
               float threshold,
               bool positive) {
  for (uint32_t i = 0; i < MAX_PADS; i++) {
    asw::input::bind_action(
        action, ControllerAxisBinding{axis, i, threshold, positive});
  }
}

void bind_key(const char* action, Key key) {
  asw::input::bind_action(action, KeyBinding{key});
}

void bind_mouse(const char* action, MouseButton button) {
  asw::input::bind_action(action, MouseButtonBinding{button});
}

bool any_pad_input() {
  const auto count = static_cast<uint32_t>(asw::input::get_controller_count());
  for (uint32_t i = 0; i < count; i++) {
    for (int b = 0; b < asw::input::NUM_CONTROLLER_BUTTONS; b++) {
      if (asw::input::get_controller_button(
              i, static_cast<ControllerButton>(b))) {
        return true;
      }
    }
    for (int a = 0; a < asw::input::NUM_CONTROLLER_AXES; a++) {
      if (std::abs(asw::input::get_controller_axis(
              i, static_cast<ControllerAxis>(a))) > 0.5F) {
        return true;
      }
    }
  }
  return false;
}

}  // namespace

void controls::bind() {
  asw::input::clear_actions();

  // Movement, analog on the left stick
  bind_key("left", Key::A);
  bind_key("left", Key::Left);
  bind_button("left", ControllerButton::DPadLeft);
  bind_axis("left", ControllerAxis::LeftX, STICK_DEAD_ZONE, false);

  bind_key("right", Key::D);
  bind_key("right", Key::Right);
  bind_button("right", ControllerButton::DPadRight);
  bind_axis("right", ControllerAxis::LeftX, STICK_DEAD_ZONE, true);

  bind_key("jump", Key::W);
  bind_key("jump", Key::Space);
  bind_key("jump", Key::Up);
  bind_button("jump", ControllerButton::A);

  bind_mouse("fire", MouseButton::Left);
  bind_button("fire", ControllerButton::RightShoulder);
  bind_axis("fire", ControllerAxis::RightTrigger, TRIGGER_THRESHOLD, true);

  bind_key("dash", Key::LShift);
  bind_key("dash", Key::RShift);
  bind_mouse("dash", MouseButton::Right);
  bind_button("dash", ControllerButton::LeftShoulder);
  bind_button("dash", ControllerButton::B);
  bind_axis("dash", ControllerAxis::LeftTrigger, TRIGGER_THRESHOLD, true);

  // Menus
  bind_key("pause", Key::P);
  bind_key("pause", Key::Escape);
  bind_button("pause", ControllerButton::Start);

  bind_key("confirm", Key::Return);
  bind_key("confirm", Key::Space);
  bind_mouse("confirm", MouseButton::Left);
  bind_button("confirm", ControllerButton::A);
  bind_button("confirm", ControllerButton::Start);

  bind_key("back", Key::Escape);
  bind_button("back", ControllerButton::B);
  bind_button("back", ControllerButton::Back);

  bind_key("quit", Key::Q);
  bind_button("quit", ControllerButton::Back);
}

void controls::update() {
  const auto& mouse = asw::input::get_mouse();
  if (mouse.any_pressed || mouse.change.x != 0.0F || mouse.change.y != 0.0F ||
      asw::input::get_keyboard().any_pressed) {
    pad_active = false;
  } else if (any_pad_input()) {
    pad_active = true;
  }
}

bool controls::using_pad() {
  return pad_active;
}

asw::Vec2f controls::aim_stick() {
  const auto count = static_cast<uint32_t>(asw::input::get_controller_count());
  for (uint32_t i = 0; i < count; i++) {
    const asw::Vec2f stick(
        asw::input::get_controller_axis(i, ControllerAxis::RightX),
        asw::input::get_controller_axis(i, ControllerAxis::RightY));
    if (std::hypot(stick.x, stick.y) > STICK_DEAD_ZONE) {
      return stick;
    }
  }
  return {0.0F, 0.0F};
}
