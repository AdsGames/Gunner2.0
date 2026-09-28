#pragma once

#include <asw/asw.h>

#include <array>
#include <string>

class World;

struct Bullet {
  asw::Vec2f pos;
  asw::Vec2f vel;
  float radius{4.0F};
  float damage{10.0F};
  float life{3.0F};
  int bounces{0};
  bool from_player{true};
  bool alive{true};

  void update(float dt);
  void draw() const;
};

// Falls from a bomber, sits on the ground, then blows up near the player
struct Mine {
  asw::Vec2f pos;
  asw::Vec2f vel;
  float fuse{5.0F};
  float blink{0.0F};
  bool landed{false};
  bool alive{true};

  static constexpr float WIDTH = 45.0F;
  static constexpr float HEIGHT = 20.0F;
  static constexpr float TRIGGER_RANGE = 70.0F;
  static constexpr float BLAST_RADIUS = 110.0F;

  asw::Quad<float> get_bounds() const {
    return {pos.x - (WIDTH / 2.0F), pos.y - HEIGHT, WIDTH, HEIGHT};
  }

  asw::Vec2f get_center() const { return {pos.x, pos.y - (HEIGHT / 2.0F)}; }

  // Returns true when it is ready to explode
  bool update(float dt, World& world);
  void draw() const;
};

enum class PickupType {
  Health,
  RapidFire,
  Ricochet,
  Spread,
  Laser,
};

inline constexpr int PICKUP_TYPE_COUNT = 5;

struct PickupInfo {
  std::string texture;
  std::string label;
  asw::Color color;
};

const PickupInfo& get_pickup_info(PickupType type);

// Crate that parachutes down from a destroyed helicopter
struct Pickup {
  PickupType type{PickupType::Health};
  asw::Vec2f pos;
  float vel_y{0.0F};
  float life{9.0F};
  float sway{0.0F};
  bool landed{false};
  bool alive{true};

  asw::Quad<float> get_bounds() const;

  void update(float dt);
  void draw() const;
};
