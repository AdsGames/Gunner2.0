#pragma once

#include <asw/asw.h>

class World;

enum class HeliType {
  Scout,
  Gunship,
  Bomber,
  Boss,
};

class Helicopter {
 public:
  Helicopter(HeliType type, int wave);

  void update(float dt, World& world);
  void draw() const;

  // Returns true if this hit killed it
  bool damage(float amount);

  asw::Vec2f get_center() const { return pos; }
  asw::Vec2f get_size() const;
  asw::Quad<float> get_hitbox() const;

  HeliType get_type() const { return type; }
  float get_health() const { return health; }
  float get_max_health() const { return max_health; }
  int get_score() const;
  bool is_alive() const { return health > 0.0F; }

 private:
  void fire_aimed(World& world, float spread, int count, float speed);
  void fire_ring(World& world, int count, float speed, float offset);
  void update_boss_attacks(float dt, World& world);

  asw::Vec2f gun_position() const;

  HeliType type;
  int wave;

  asw::Vec2f pos;
  float base_y{0.0F};
  float target_y{0.0F};
  float speed{0.0F};
  float dir{1.0F};
  float scale{1.0F};
  asw::Color tint;

  float health{0.0F};
  float max_health{0.0F};

  float fire_timer{0.0F};
  float fire_delay{0.0F};
  float mine_timer{0.0F};
  float hover_timer{0.0F};
  float retarget_timer{0.0F};
  float hurt_timer{0.0F};
  float bob{0.0F};
  float tilt{0.0F};
  bool entering{true};

  // Boss attack state
  float spiral_time{0.0F};
  float spiral_angle{0.0F};
  float spiral_shot_timer{0.0F};
  float ring_timer{0.0F};
};
