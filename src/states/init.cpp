#include "./init.h"

#include <array>
#include <string>

#include "../controls.h"

void Init::init() {
  asw::display::set_title("Setting up");

  const std::array<std::string, 14> textures = {
      "background",      "box_laserbeam",  "crate",     "crate_health",
      "crate_rapidfire", "crate_ricochet", "cursor",    "helicopter",
      "helicopter_hurt", "icon",           "laserbeam", "mine",
      "player",          "player_hurt",
  };
  for (const auto& name : textures) {
    asw::assets::load_texture("assets/images/" + name + ".png", name);
  }

  const std::array<std::string, 15> samples = {
      "shoot",         "enemy_shoot", "laser", "hit",      "explosion",
      "big_explosion", "pickup",      "jump",  "hurt",     "select",
      "combo",         "wave",        "boss",  "gameover", "mine",
  };
  for (const auto& name : samples) {
    asw::assets::load_sample("assets/sounds/" + name + ".wav", name);
  }

  asw::assets::load_music("assets/sounds/music.wav", "music");

  const std::string font = "assets/fonts/PressStart2P-Regular.ttf";
  asw::assets::load_font(font, 12, "font_s", asw::FontStyle::Pixel);
  asw::assets::load_font(font, 16, "font_m", asw::FontStyle::Pixel);
  asw::assets::load_font(font, 24, "font_l", asw::FontStyle::Pixel);
  asw::assets::load_font(font, 48, "font_xl", asw::FontStyle::Pixel);
  asw::assets::load_font(font, 72, "font_title", asw::FontStyle::Pixel);

  asw::display::set_icon("assets/images/icon.png");
  asw::display::set_title("Gunner 2.0");

  // Primitives use alpha for particles, flashes and overlays
  asw::display::set_blend_mode(asw::BlendMode::Blend);

  controls::bind();

  // The game draws its own crosshair
  SDL_HideCursor();

  asw::sound::play_music(asw::assets::get_music("music"), 0.5F);
}

void Init::update(float /*dt*/) {
  manager.set_next_scene(ProgramState::Menu);
}

void Init::draw() {
  asw::draw::clear_color(asw::Color(0, 0, 0));
}
