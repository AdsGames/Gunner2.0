#pragma once

#include <asw/asw.h>

#include <string>
#include <vector>

// Particles, floating score text, screen shake and flashes
class Effects {
 public:
  Effects();

  void update(float dt);

  // Drawn inside the world canvas
  void draw_particles() const;
  void draw_text() const;

  // Drawn over everything
  void draw_flash() const;

  void clear();

  // Fireball, smoke and debris. Scale 1 fits a small helicopter.
  void explosion(const asw::Vec2f& pos, float scale);

  // Chain of explosions spread over time, for big kills
  void chain_explosion(const asw::Vec2f& pos,
                       const asw::Vec2f& spread,
                       int count,
                       float duration);

  // Short burst of sparks, angle is the main direction
  void sparks(const asw::Vec2f& pos,
              asw::Color color,
              int count,
              float angle,
              float spread,
              float speed);

  void muzzle_flash(const asw::Vec2f& pos, float angle);

  // Single rising puff of dark smoke
  void smoke_puff(const asw::Vec2f& pos);

  void float_text(const asw::Vec2f& pos,
                  const std::string& text,
                  asw::Color color,
                  bool big = false);

  void shake(float amount);
  void flash(asw::Color color, float duration);

  // Fixed view that only moves to shake the world layer
  const asw::Camera& get_camera() const { return camera; }

 private:
  struct Particle {
    asw::Vec2f pos;
    asw::Vec2f vel;
    float life{0.0F};
    float max_life{1.0F};
    float size_start{4.0F};
    float size_end{0.0F};
    asw::Color color_start;
    asw::Color color_end;
    float drag{0.0F};
    float gravity{0.0F};
  };

  struct FloatText {
    asw::Vec2f pos;
    std::string text;
    asw::Color color;
    float life{0.0F};
    float max_life{1.0F};
    bool big{false};
  };

  struct PendingExplosion {
    asw::Vec2f pos;
    float delay{0.0F};
    float scale{1.0F};
  };

  void add(const Particle& particle);

  std::vector<Particle> particles;
  std::vector<FloatText> texts;
  std::vector<PendingExplosion> pending;

  asw::Camera camera;

  asw::Color flash_color;
  float flash_time{0.0F};
  float flash_max{1.0F};
};
