#pragma once

#include <asw/asw.h>

// Named input actions shared by keyboard, mouse and gamepads
namespace controls {

// Register every action binding. Call once after asw is initialised.
void bind();

// Track which device the player touched last. Call once per update.
void update();

// True when a gamepad was used more recently than the mouse or keyboard
bool using_pad();

// Right stick direction of the first controller pushing it, with a radial
// dead zone applied. Zero when no stick is pushed.
asw::Vec2f aim_stick();

}  // namespace controls
