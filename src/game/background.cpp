#include "./background.h"

#include "../globals.h"

Background::Background() {
  for (int i = 0; i < 6; i++) {
    clouds.push_back({{asw::random::between(0.0F, static_cast<float>(SCREEN_W)),
                       asw::random::between(30.0F, 170.0F)},
                      asw::random::between(8.0F, 25.0F),
                      asw::random::between(0.6F, 1.4F)});
  }
}

void Background::update(float dt) {
  for (auto& c : clouds) {
    c.pos.x += c.speed * dt;
    if (c.pos.x - (120.0F * c.scale) > SCREEN_W) {
      c.pos.x = -120.0F * c.scale;
      c.pos.y = asw::random::between(30.0F, 170.0F);
    }
  }
}

void Background::draw() const {
  asw::draw::sprite(asw::assets::get_texture("background"), asw::Vec2f(0, 0));

  // Blocky clouds to match the pixel font
  const asw::Color cloud(245, 255, 255);
  for (const auto& c : clouds) {
    const float s = c.scale;
    asw::draw::rect_fill(
        asw::Quad<float>(c.pos.x - (60 * s), c.pos.y, 120 * s, 20 * s), cloud);
    asw::draw::rect_fill(asw::Quad<float>(c.pos.x - (35 * s),
                                          c.pos.y - (14 * s), 60 * s, 16 * s),
                         cloud);
    asw::draw::rect_fill(
        asw::Quad<float>(c.pos.x + (5 * s), c.pos.y - (24 * s), 34 * s, 26 * s),
        cloud);
  }
}
