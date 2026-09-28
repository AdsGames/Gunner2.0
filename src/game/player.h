#pragma once

#include <asw/asw.h>

#include <array>
#include <vector>

#include "./entities.h"

class World;

class Player {
 public:
  static constexpr float WIDTH = 49.0F;
  static constexpr float HEIGHT = 45.0F;
  static constexpr float MAX_HEALTH = 100.0F;

  void reset();

  void update(float dt, World& world);
  void draw() const;

  // Returns true if the hit landed
  bool hurt(float damage, World& world, const asw::Vec2f& from);
  void heal(float amount);
  void give_power(PickupType type);

  // Respawn after losing a life
  void respawn();

  asw::Vec2f get_center() const {
    return {pos.x + (WIDTH / 2.0F), pos.y + (HEIGHT / 2.0F)};
  }

  // Smaller than the sprite so near misses feel fair
  asw::Quad<float> get_hitbox() const {
    return {pos.x + 12.0F, pos.y + 8.0F, WIDTH - 24.0F, HEIGHT - 10.0F};
  }

  asw::Quad<float> get_pickup_box() const { return {pos, {WIDTH, HEIGHT}}; }

  asw::Vec2f get_gun_tip() const;

  float get_health() const { return health; }
  bool is_alive() const { return alive; }
  bool is_invulnerable() const { return invuln_timer > 0.0F || dashing(); }
  float get_power_time(PickupType type) const {
    return power_timers.at(static_cast<size_t>(type));
  }
  float get_dash_ready() const;
  bool is_firing_laser() const { return laser_on; }
  float get_aim() const { return aim; }

 private:
  bool dashing() const { return dash_timer > 0.0F; }
  void fire(World& world);
  void update_laser(float dt, World& world);

  asw::Vec2f pos;
  asw::Vec2f vel;

  float health{MAX_HEALTH};
  bool alive{true};
  bool on_ground{true};
  int jumps_left{2};

  float aim{0.0F};
  bool facing_left{false};

  float fire_timer{0.0F};
  float hurt_timer{0.0F};
  float invuln_timer{0.0F};
  float dash_timer{0.0F};
  float dash_cooldown{0.0F};
  float dash_dir{1.0F};
  float laser_sound_timer{0.0F};
  bool laser_on{false};

  // Afterimages left behind while dashing
  struct Ghost {
    asw::Vec2f pos;
    float life;
    bool facing_left;
  };
  std::vector<Ghost> ghosts;

  std::array<float, PICKUP_TYPE_COUNT> power_timers{};
};
