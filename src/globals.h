#pragma once

#include <asw/asw.h>

#include <numbers>
#include <string>

// Logical screen size
inline constexpr int SCREEN_W = 1024;
inline constexpr int SCREEN_H = 768;

// Feet line of anything standing on the ground
inline constexpr float GROUND_Y = 760.0F;

// Longest frame time simulated at once, in seconds
inline constexpr float MAX_DT = 0.05F;

inline constexpr float PI = std::numbers::pi_v<float>;

// Shared helpers
namespace gfx {

// Draw a texture stretched to dest, rotated around its centre and optionally
// mirrored. asw has no single call for flip + scale + rotate.
void sprite_ex(const asw::Texture& tex,
               const asw::Quad<float>& dest,
               float angle,
               bool flip_x);

// Draw text with a hard drop shadow, arcade style
void text_shadow(const std::string& font_key,
                 const std::string& str,
                 const asw::Vec2f& pos,
                 asw::Color color,
                 asw::TextJustify justify = asw::TextJustify::Left,
                 float shadow = 3.0F);

}  // namespace gfx

namespace audio {

// Play a cached sample, panned by screen x position
void play(const std::string& key,
          float volume = 1.0F,
          float x = SCREEN_W / 2.0F);

}  // namespace audio

namespace highscore {

int load();
void save(int score);

}  // namespace highscore
