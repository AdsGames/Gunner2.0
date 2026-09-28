#pragma once

#include <asw/asw.h>

// Named input actions shared by keyboard, mouse and gamepads
namespace controls {

// Register every action binding. Call once after asw is initialised.
void bind();

// True when a gamepad was used more recently than the mouse or keyboard
inline bool using_pad() {
  return asw::input::get_last_device() == asw::input::InputDevice::Controller;
}

}  // namespace controls
