#include "./effects.h"

#include <algorithm>
#include <cmath>

#include "../globals.h"

namespace {

constexpr size_t MAX_PARTICLES = 2500;

}  // namespace

void Effects::add(const Particle& particle) {
  if (particles.size() < MAX_PARTICLES) {
    particles.push_back(particle);
  }
}

void Effects::update(float dt) {
  for (auto& p : particles) {
    p.life -= dt;
    p.vel.y += p.gravity * dt;
    p.vel *= std::max(0.0F, 1.0F - (p.drag * dt));
    p.pos += p.vel * dt;
  }
  std::erase_if(particles, [](const Particle& p) { return p.life <= 0.0F; });

  for (auto& t : texts) {
    t.life -= dt;
    t.pos.y -= (t.big ? 30.0F : 60.0F) * dt;
  }
  std::erase_if(texts, [](const FloatText& t) { return t.life <= 0.0F; });

  // Pending chain explosions
  std::vector<PendingExplosion> ready;
  for (auto& e : pending) {
    e.delay -= dt;
    if (e.delay <= 0.0F) {
      ready.push_back(e);
    }
  }
  std::erase_if(pending,
                [](const PendingExplosion& e) { return e.delay <= 0.0F; });
  for (const auto& e : ready) {
    explosion(e.pos, e.scale);
    shake(8.0F * e.scale);
    audio::play("explosion", 0.5F, e.pos.x);
  }

  // Shake decays quickly and jitters every frame
  shake_amount =
      std::max(0.0F, shake_amount - (shake_amount * 8.0F * dt) - (10.0F * dt));
  if (shake_amount > 0.0F) {
    shake_offset = {asw::random::between(-shake_amount, shake_amount),
                    asw::random::between(-shake_amount, shake_amount)};
  } else {
    shake_offset = {0.0F, 0.0F};
  }

  flash_time = std::max(0.0F, flash_time - dt);
}

void Effects::draw_particles() const {
  for (const auto& p : particles) {
    const float t = 1.0F - (p.life / p.max_life);
    const float size = p.size_start + ((p.size_end - p.size_start) * t);
    if (size < 0.5F) {
      continue;
    }
    // Square particles suit the chunky pixel look
    asw::draw::rect_fill(asw::Quad<float>(p.pos.x - (size / 2.0F),
                                          p.pos.y - (size / 2.0F), size, size),
                         p.color_start.lerp(p.color_end, t));
  }
}

void Effects::draw_text() const {
  for (const auto& t : texts) {
    const float fade = std::clamp(t.life / (t.max_life * 0.4F), 0.0F, 1.0F);
    const auto alpha = static_cast<uint8_t>(255.0F * fade);
    gfx::text_shadow(t.big ? "font_l" : "font_s", t.text, t.pos,
                     t.color.with_alpha(alpha), asw::TextJustify::Center,
                     t.big ? 3.0F : 2.0F);
  }
}

void Effects::draw_flash() const {
  if (flash_time <= 0.0F) {
    return;
  }
  const float t = flash_time / flash_max;
  asw::draw::rect_fill(
      asw::Quad<float>(0, 0, SCREEN_W, SCREEN_H),
      flash_color.with_alpha(static_cast<uint8_t>(flash_color.a * t)));
}

void Effects::clear() {
  particles.clear();
  texts.clear();
  pending.clear();
  shake_amount = 0.0F;
  shake_offset = {0.0F, 0.0F};
  flash_time = 0.0F;
}

void Effects::explosion(const asw::Vec2f& pos, float scale) {
  // Smoke first so the fire draws over it
  for (int i = 0; i < static_cast<int>(14 * scale); i++) {
    Particle p;
    p.pos = pos + asw::Vec2f(asw::random::between(-20.0F, 20.0F) * scale,
                             asw::random::between(-15.0F, 15.0F) * scale);
    p.vel = asw::Vec2f::from_angle(asw::random::between(0.0F, 2.0F * PI),
                                   asw::random::between(10.0F, 80.0F));
    p.max_life = p.life = asw::random::between(0.8F, 1.6F);
    p.size_start = asw::random::between(10.0F, 20.0F) * std::sqrt(scale);
    p.size_end = asw::random::between(26.0F, 40.0F) * std::sqrt(scale);
    p.color_start = asw::Color(90, 80, 70, 220);
    p.color_end = asw::Color(60, 60, 60, 0);
    p.drag = 1.5F;
    p.gravity = -70.0F;
    add(p);
  }

  // Debris
  for (int i = 0; i < static_cast<int>(12 * scale); i++) {
    Particle p;
    p.pos = pos;
    p.vel = asw::Vec2f::from_angle(asw::random::between(PI, 2.0F * PI),
                                   asw::random::between(150.0F, 450.0F));
    p.max_life = p.life = asw::random::between(0.8F, 1.6F);
    p.size_start = asw::random::between(3.0F, 7.0F);
    p.size_end = p.size_start;
    p.color_start = asw::Color(40, 60, 40);
    p.color_end = asw::Color(30, 30, 30, 0);
    p.gravity = 900.0F;
    add(p);
  }

  // Fireballs
  for (int i = 0; i < static_cast<int>(24 * scale); i++) {
    Particle p;
    p.pos = pos;
    p.vel = asw::Vec2f::from_angle(
        asw::random::between(0.0F, 2.0F * PI),
        asw::random::between(40.0F, 260.0F) * std::sqrt(scale));
    p.max_life = p.life = asw::random::between(0.3F, 0.7F);
    p.size_start = asw::random::between(12.0F, 24.0F) * std::sqrt(scale);
    p.size_end = 2.0F;
    p.color_start = asw::Color(255, 230, 90);
    p.color_end = asw::Color(200, 40, 10, 0);
    p.drag = 3.0F;
    p.gravity = -60.0F;
    add(p);
  }

  // Flash core
  for (int i = 0; i < static_cast<int>(6 * scale) + 2; i++) {
    Particle p;
    p.pos = pos + asw::Vec2f(asw::random::between(-10.0F, 10.0F) * scale,
                             asw::random::between(-10.0F, 10.0F) * scale);
    p.vel = asw::Vec2f::from_angle(asw::random::between(0.0F, 2.0F * PI),
                                   asw::random::between(10.0F, 60.0F));
    p.max_life = p.life = asw::random::between(0.12F, 0.25F);
    p.size_start = asw::random::between(30.0F, 50.0F) * scale;
    p.size_end = 10.0F * scale;
    p.color_start = asw::Color(255, 255, 220);
    p.color_end = asw::Color(255, 200, 60, 200);
    add(p);
  }
}

void Effects::chain_explosion(const asw::Vec2f& pos,
                              const asw::Vec2f& spread,
                              int count,
                              float duration) {
  for (int i = 0; i < count; i++) {
    PendingExplosion e;
    e.pos = pos + asw::Vec2f(asw::random::between(-spread.x, spread.x),
                             asw::random::between(-spread.y, spread.y));
    e.delay = duration * static_cast<float>(i) / static_cast<float>(count);
    e.scale = asw::random::between(0.7F, 1.4F);
    pending.push_back(e);
  }
}

void Effects::sparks(const asw::Vec2f& pos,
                     asw::Color color,
                     int count,
                     float angle,
                     float spread,
                     float speed) {
  for (int i = 0; i < count; i++) {
    Particle p;
    p.pos = pos;
    p.vel =
        asw::Vec2f::from_angle(angle + asw::random::between(-spread, spread),
                               asw::random::between(speed * 0.3F, speed));
    p.max_life = p.life = asw::random::between(0.15F, 0.4F);
    p.size_start = asw::random::between(3.0F, 5.0F);
    p.size_end = 1.0F;
    p.color_start = color;
    p.color_end = color.with_alpha(0);
    p.drag = 4.0F;
    p.gravity = 400.0F;
    add(p);
  }
}

void Effects::muzzle_flash(const asw::Vec2f& pos, float angle) {
  for (int i = 0; i < 3; i++) {
    Particle p;
    p.pos = pos;
    p.vel = asw::Vec2f::from_angle(angle + asw::random::between(-0.3F, 0.3F),
                                   asw::random::between(100.0F, 300.0F));
    p.max_life = p.life = 0.06F;
    p.size_start = asw::random::between(6.0F, 10.0F);
    p.size_end = 2.0F;
    p.color_start = asw::Color(255, 255, 200);
    p.color_end = asw::Color(255, 180, 40, 0);
    add(p);
  }
}

void Effects::smoke_puff(const asw::Vec2f& pos) {
  Particle p;
  p.pos = pos;
  p.vel = {asw::random::between(-20.0F, 20.0F),
           asw::random::between(-60.0F, -20.0F)};
  p.max_life = p.life = asw::random::between(0.6F, 1.1F);
  p.size_start = asw::random::between(6.0F, 10.0F);
  p.size_end = asw::random::between(16.0F, 24.0F);
  p.color_start = asw::Color(70, 70, 70, 200);
  p.color_end = asw::Color(120, 120, 120, 0);
  p.drag = 1.0F;
  add(p);
}

void Effects::float_text(const asw::Vec2f& pos,
                         const std::string& text,
                         asw::Color color,
                         bool big) {
  FloatText t;
  t.pos = pos;
  t.text = text;
  t.color = color;
  t.max_life = t.life = big ? 1.6F : 0.9F;
  t.big = big;
  texts.push_back(t);
}

void Effects::shake(float amount) {
  shake_amount = std::min(30.0F, std::max(shake_amount, amount));
}

void Effects::flash(asw::Color color, float duration) {
  flash_color = color;
  flash_time = duration;
  flash_max = duration;
}
