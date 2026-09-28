#include "./game.h"

#include <algorithm>
#include <cmath>

#include "../controls.h"
#include "../globals.h"

void Game::init() {
  canvas = asw::assets::create_texture(SCREEN_W, SCREEN_H);
  restart();
}

void Game::restart() {
  high_score = highscore::load();
  world.reset(high_score);
  paused = false;
  game_over_time = 0.0F;
  new_high_score = false;
}

void Game::update(float dt) {
  using asw::input::get_action_down;

  dt = std::min(dt, MAX_DT);
  time += dt;

  if (world.is_game_over()) {
    // Save once, the moment the game ends
    if (game_over_time == 0.0F && world.get_score() > high_score) {
      new_high_score = true;
      highscore::save(world.get_score());
    }
    game_over_time += dt;
    world.update(dt);

    // Short delay so a held fire button does not skip the screen
    if (game_over_time > 1.2F) {
      if (get_action_down("confirm")) {
        audio::play("select");
        restart();
      } else if (get_action_down("back")) {
        manager.set_next_scene(ProgramState::Menu);
      }
    }
    return;
  }

  if (get_action_down("pause")) {
    paused = !paused;
    audio::play("select");
    world.get_player().pause_sounds(paused);
    if (paused) {
      asw::sound::pause_music();
    } else {
      asw::sound::resume_music();
    }
  }

  if (paused) {
    if (get_action_down("quit")) {
      world.get_player().stop_sounds();
      asw::sound::resume_music();
      manager.set_next_scene(ProgramState::Menu);
    }
    return;
  }

  world.update(dt);
}

void Game::draw() {
  // World to the canvas
  asw::display::set_render_target(canvas);
  asw::draw::clear_color(asw::Color(0, 0, 0));
  world.draw();
  asw::display::reset_render_target();

  // Canvas to the screen with shake
  const auto& camera = world.get_effects().get_camera();
  asw::draw::clear_color(asw::Color(0, 0, 0));
  asw::draw::stretch_sprite(
      canvas,
      camera.world_to_screen(asw::Quad<float>(0, 0, SCREEN_W, SCREEN_H)));

  world.draw_hud();
  world.get_effects().draw_flash();

  if (world.is_game_over()) {
    draw_game_over();
  } else if (paused) {
    draw_paused();
  }

  // Crosshair, out along the aim when on a gamepad
  auto& player = world.get_player();
  if (!controls::using_pad()) {
    const auto mouse = asw::input::get_mouse().position;
    asw::draw::sprite(asw::assets::get_texture("cursor"),
                      mouse - asw::Vec2f(20, 20));
  } else if (player.is_alive() && !world.is_game_over() && !paused) {
    const float aim = player.get_aim();
    const auto target =
        player.get_center() + asw::Vec2f::from_angle(aim, 160.0F);
    asw::draw::sprite(asw::assets::get_texture("cursor"),
                      target - asw::Vec2f(20, 20));
  }
}

void Game::draw_paused() const {
  asw::draw::rect_fill(asw::Quad<float>(0, 0, SCREEN_W, SCREEN_H),
                       asw::Color(0, 0, 0, 150));
  gfx::text_shadow("font_xl", "PAUSED", asw::Vec2f(SCREEN_W / 2.0F, 300),
                   asw::Color(255, 220, 60), asw::TextJustify::Center, 5.0F);
  const bool pad = controls::using_pad();
  gfx::text_shadow("font_m", pad ? "START TO RESUME" : "P / ESC TO RESUME",
                   asw::Vec2f(SCREEN_W / 2.0F, 390), asw::Color(255, 255, 255),
                   asw::TextJustify::Center);
  gfx::text_shadow("font_m", pad ? "BACK TO QUIT TO MENU" : "Q TO QUIT TO MENU",
                   asw::Vec2f(SCREEN_W / 2.0F, 425), asw::Color(255, 255, 255),
                   asw::TextJustify::Center);
}

void Game::draw_game_over() const {
  const float fade = std::min(1.0F, game_over_time / 0.8F);
  asw::draw::rect_fill(asw::Quad<float>(0, 0, SCREEN_W, SCREEN_H),
                       asw::Color(0, 0, 0, static_cast<uint8_t>(170 * fade)));

  const float drop = asw::easing::ease(-200.0F, 230.0F, game_over_time / 0.8F,
                                       asw::easing::ease_out_bounce);
  gfx::text_shadow("font_xl", "GAME OVER", asw::Vec2f(SCREEN_W / 2.0F, drop),
                   asw::Color(255, 70, 50), asw::TextJustify::Center, 5.0F);

  if (game_over_time < 0.8F) {
    return;
  }

  gfx::text_shadow("font_l", "SCORE " + std::to_string(world.get_score()),
                   asw::Vec2f(SCREEN_W / 2.0F, 330), asw::Color(255, 255, 255),
                   asw::TextJustify::Center);
  gfx::text_shadow("font_m", "REACHED WAVE " + std::to_string(world.get_wave()),
                   asw::Vec2f(SCREEN_W / 2.0F, 375), asw::Color(200, 200, 200),
                   asw::TextJustify::Center);

  if (new_high_score && std::fmod(time, 0.5F) < 0.3F) {
    gfx::text_shadow("font_l", "NEW HIGH SCORE!",
                     asw::Vec2f(SCREEN_W / 2.0F, 430), asw::Color(255, 220, 60),
                     asw::TextJustify::Center);
  }

  if (game_over_time > 1.2F) {
    const bool pad = controls::using_pad();
    gfx::text_shadow(
        "font_m",
        pad ? "PRESS A TO PLAY AGAIN" : "CLICK OR ENTER TO PLAY AGAIN",
        asw::Vec2f(SCREEN_W / 2.0F, 520), asw::Color(255, 255, 255),
        asw::TextJustify::Center);
    gfx::text_shadow("font_m", pad ? "B FOR MENU" : "ESC FOR MENU",
                     asw::Vec2f(SCREEN_W / 2.0F, 555),
                     asw::Color(255, 255, 255), asw::TextJustify::Center);
  }
}
