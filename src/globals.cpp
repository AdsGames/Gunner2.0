#include "./globals.h"

#include <algorithm>
#include <fstream>

void gfx::sprite_ex(const asw::Texture& tex,
                    const asw::Quad<float>& dest,
                    float angle,
                    bool flip_x) {
  auto* renderer = asw::display::get_renderer();
  if (renderer == nullptr || tex == nullptr) {
    return;
  }

  const SDL_FRect rect{dest.position.x, dest.position.y, dest.size.x,
                       dest.size.y};
  const double degrees = angle * (180.0 / std::numbers::pi);

  SDL_RenderTextureRotated(renderer, tex.get(), nullptr, &rect, degrees,
                           nullptr,
                           flip_x ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
}

void gfx::text_shadow(const std::string& font_key,
                      const std::string& str,
                      const asw::Vec2f& pos,
                      asw::Color color,
                      asw::TextJustify justify,
                      float shadow) {
  const auto font = asw::assets::get_font(font_key);
  asw::draw::text(font, str, pos + asw::Vec2f(shadow, shadow),
                  asw::Color(0, 0, 0, color.a), justify);
  asw::draw::text(font, str, pos, color, justify);
}

void audio::play(const std::string& key, float volume, float x) {
  const float pan = std::clamp(((x / SCREEN_W) * 2.0F) - 1.0F, -1.0F, 1.0F);
  asw::sound::play(asw::assets::get_sample(key), volume, pan * 0.6F);
}

namespace {

std::string highscore_path() {
  const auto folder = asw::assets::get_save_path("adsgames", "gunner");
  if (folder.empty()) {
    return "";
  }
  return folder + "highscore.txt";
}

}  // namespace

int highscore::load() {
  const auto path = highscore_path();
  if (path.empty()) {
    return 0;
  }

  std::ifstream file(path);
  int score = 0;
  if (file >> score) {
    return std::max(score, 0);
  }
  return 0;
}

void highscore::save(int score) {
  const auto path = highscore_path();
  if (path.empty()) {
    return;
  }

  std::ofstream file(path, std::ios::trunc);
  file << score;
}
