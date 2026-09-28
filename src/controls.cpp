#include "./controls.h"

namespace {

using asw::input::ANY_CONTROLLER;
using asw::input::ControllerAxis;
using asw::input::ControllerAxisBinding;
using asw::input::ControllerButton;
using asw::input::ControllerButtonBinding;
using asw::input::Key;
using asw::input::KeyBinding;
using asw::input::MouseButton;
using asw::input::MouseButtonBinding;

// Axes already have asw's dead zone applied, so a small push moves the player
constexpr float STICK_THRESHOLD = 0.05F;
constexpr float TRIGGER_THRESHOLD = 0.3F;

void bind_button(const char* action, ControllerButton button) {
  asw::input::bind_action(action,
                          ControllerButtonBinding{button, ANY_CONTROLLER});
}

void bind_axis(const char* action,
               ControllerAxis axis,
               float threshold,
               bool positive) {
  asw::input::bind_action(
      action, ControllerAxisBinding{axis, ANY_CONTROLLER, threshold, positive});
}

void bind_key(const char* action, Key key) {
  asw::input::bind_action(action, KeyBinding{key});
}

void bind_mouse(const char* action, MouseButton button) {
  asw::input::bind_action(action, MouseButtonBinding{button});
}

}  // namespace

void controls::bind() {
  asw::input::clear_actions();

  // Movement, analog on the left stick
  bind_key("left", Key::A);
  bind_key("left", Key::Left);
  bind_button("left", ControllerButton::DPadLeft);
  bind_axis("left", ControllerAxis::LeftX, STICK_THRESHOLD, false);

  bind_key("right", Key::D);
  bind_key("right", Key::Right);
  bind_button("right", ControllerButton::DPadRight);
  bind_axis("right", ControllerAxis::LeftX, STICK_THRESHOLD, true);

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
