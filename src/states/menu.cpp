#include "./menu.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <utility>

#include "../controls.h"
#include "../globals.h"

void Menu::init() {
  time = 0.0F;
  flyby_timer = 0.0F;
  flybys.clear();
  high_score = highscore::load();
  add_flyby();
}

void Menu::add_flyby() {
  const bool right = asw::random::chance();
  const float scale = asw::random::between(0.5F, 1.1F);
  const float speed = asw::random::between(120.0F, 260.0F);
  const std::array<asw::Color, 3> tints = {asw::Color(255, 255, 255),
                                           asw::Color(255, 225, 150),
                                           asw::Color(170, 175, 215)};
  flybys.push_back({{right ? -200.0F : SCREEN_W + 200.0F,
                     asw::random::between(60.0F, 420.0F)},
                    right ? speed : -speed,
                    scale,
                    tints.at(static_cast<size_t>(asw::random::between(0, 2)))});
}

void Menu::update(float dt) {
  controls::update();

  dt = std::min(dt, MAX_DT);
  time += dt;
  background.update(dt);

  flyby_timer -= dt;
  if (flyby_timer <= 0.0F) {
    flyby_timer = asw::random::between(1.5F, 3.5F);
    add_flyby();
  }
  for (auto& f : flybys) {
    f.pos.x += f.speed * dt;
  }
  std::erase_if(flybys, [](const Flyby& f) {
    return f.pos.x < -300.0F || f.pos.x > SCREEN_W + 300.0F;
  });

  if (asw::input::get_action_down("confirm")) {
    audio::play("select");
    manager.set_next_scene(ProgramState::Game);
  }

#ifndef __EMSCRIPTEN__
  if (asw::input::get_key_down(asw::input::Key::Escape)) {
    asw::core::exit();
  }
#endif
}

void Menu::draw() {
  background.draw();

  const auto heli = asw::assets::get_texture("helicopter");
  for (const auto& f : flybys) {
    const asw::Vec2f size(200.0F * f.scale, 62.0F * f.scale);
    const float bob = std::sin((time * 2.0F) + f.pos.x * 0.01F) * 6.0F;
    asw::draw::set_tint(heli, f.tint);
    gfx::sprite_ex(heli,
                   asw::Quad<float>(f.pos.x - (size.x / 2.0F), f.pos.y + bob,
                                    size.x, size.y),
                   f.speed > 0 ? 0.1F : -0.1F, f.speed > 0);
  }
  asw::draw::set_tint(heli, asw::Color(255, 255, 255));

  // Hero on the ground
  asw::draw::stretch_sprite(asw::assets::get_texture("player"),
                            asw::Quad<float>(SCREEN_W / 2.0F - 49.0F,
                                             GROUND_Y - 90.0F, 98.0F, 90.0F));

  // Title
  const float bob = std::sin(time * 2.5F) * 6.0F;
  gfx::text_shadow("font_title", "GUNNER",
                   asw::Vec2f(SCREEN_W / 2.0F, 170.0F + bob),
                   asw::Color(255, 220, 60), asw::TextJustify::Center, 7.0F);
  gfx::text_shadow("font_xl", "2.0",
                   asw::Vec2f(SCREEN_W / 2.0F + 250.0F, 250.0F - bob),
                   asw::Color(255, 70, 50), asw::TextJustify::Center, 5.0F);

  // Controls panel
  asw::draw::rect_fill(asw::Quad<float>(262, 330, 500, 200),
                       asw::Color(0, 0, 0, 150));
  const asw::Color label(255, 220, 60);
  const asw::Color value(255, 255, 255);
  using Bindings = std::array<std::pair<const char*, const char*>, 6>;
  const Bindings keyboard = {{
      {"MOVE", "A/D  or  ARROWS"},
      {"JUMP", "W/SPACE (x2)"},
      {"AIM", "MOUSE"},
      {"SHOOT", "LEFT CLICK"},
      {"DASH", "SHIFT / RIGHT CLICK"},
      {"PAUSE", "P / ESC"},
  }};
  const Bindings gamepad = {{
      {"MOVE", "LEFT STICK / DPAD"},
      {"JUMP", "A (x2)"},
      {"AIM", "RIGHT STICK"},
      {"SHOOT", "RT / RB"},
      {"DASH", "LT / LB / B"},
      {"PAUSE", "START"},
  }};
  const bool pad = controls::using_pad();
  float y = 350.0F;
  for (const auto& [name, keys] : pad ? gamepad : keyboard) {
    gfx::text_shadow("font_s", name, asw::Vec2f(290, y), label,
                     asw::TextJustify::Left, 2.0F);
    gfx::text_shadow("font_s", keys, asw::Vec2f(400, y), value,
                     asw::TextJustify::Left, 2.0F);
    y += 28.0F;
  }

  // Prompt
  if (std::fmod(time, 1.0F) < 0.65F) {
    gfx::text_shadow("font_m",
                     pad ? "PRESS A TO START" : "CLICK OR PRESS ENTER TO START",
                     asw::Vec2f(SCREEN_W / 2.0F, 570.0F),
                     asw::Color(255, 255, 255), asw::TextJustify::Center);
  }

  gfx::text_shadow("font_m", "HI SCORE " + std::to_string(high_score),
                   asw::Vec2f(SCREEN_W / 2.0F, 20.0F), asw::Color(255, 220, 60),
                   asw::TextJustify::Center);

  gfx::text_shadow("font_s", "A.D.S. GAMES",
                   asw::Vec2f(SCREEN_W - 16.0F, SCREEN_H - 24.0F),
                   asw::Color(255, 255, 255), asw::TextJustify::Right, 2.0F);

  // Crosshair
  if (!pad) {
    const auto mouse = asw::input::get_mouse().position;
    asw::draw::sprite(asw::assets::get_texture("cursor"),
                      mouse - asw::Vec2f(20, 20));
  }
}
