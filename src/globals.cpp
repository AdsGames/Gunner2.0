#include "./globals.h"

#include <algorithm>
#include <fstream>
#include <string_view>

void gfx::text_shadow(const std::string& font_key,
                      const std::string& str,
                      const asw::Vec2f& pos,
                      asw::Color color,
                      asw::TextJustify justify,
                      float shadow) {
  asw::draw::text_shadow(asw::assets::get_font(font_key), str, pos, color,
                         asw::Color(0, 0, 0), asw::Vec2f(shadow, shadow),
                         justify);
}

namespace {

struct SoundStyle {
  // Random pitch change per play, so repeats do not sound identical
  float pitch_variation{0.0F};
  // Big moments keep their voice when rapid fire fills every channel
  int priority{0};
  // How far to drop the music, 1 for no ducking, and for how long
  float duck_gain{1.0F};
  float duck_hold{0.0F};
};

SoundStyle style_for(std::string_view key) {
  if (key == "shoot" || key == "enemy_shoot" || key == "hit" || key == "jump" ||
      key == "mine") {
    return {.pitch_variation = 0.06F};
  }
  if (key == "explosion") {
    return {.pitch_variation = 0.08F, .priority = 1};
  }
  if (key == "big_explosion") {
    return {.pitch_variation = 0.05F,
            .priority = 2,
            .duck_gain = 0.5F,
            .duck_hold = 0.8F};
  }
  if (key == "boss") {
    return {.priority = 2, .duck_gain = 0.3F, .duck_hold = 1.3F};
  }
  if (key == "gameover") {
    return {.priority = 2, .duck_gain = 0.3F, .duck_hold = 1.6F};
  }
  if (key == "wave" || key == "hurt" || key == "pickup" || key == "combo") {
    return {.priority = 1};
  }
  return {};
}

}  // namespace

float audio::pan_at(float x) {
  // Same curve as asw::sound::play_at
  const float center = SCREEN_W / 2.0F;
  return std::clamp(((x - center) / center) * 0.7F, -1.0F, 1.0F);
}

void audio::play(const std::string& key, float volume, float x) {
  const auto style = style_for(key);

  asw::sound::PlayOptions options;
  options.volume = volume;
  options.pan = pan_at(x);
  options.pitch_variation = style.pitch_variation;
  options.priority = style.priority;
  asw::sound::play(asw::assets::get_sample(key), options);

  if (style.duck_gain < 1.0F) {
    asw::sound::duck(asw::sound::Bus::Music, style.duck_gain, style.duck_hold);
  }
}

asw::sound::SoundHandle audio::loop(const std::string& key,
                                    float volume,
                                    float x) {
  asw::sound::PlayOptions options;
  options.volume = volume;
  options.pan = pan_at(x);
  options.loop = true;
  options.priority = 1;
  options.fade_in_s = 0.05F;
  return asw::sound::play(asw::assets::get_sample(key), options);
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
