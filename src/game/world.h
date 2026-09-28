#pragma once

#include <asw/asw.h>

#include <string>
#include <vector>

#include "./background.h"
#include "./effects.h"
#include "./entities.h"
#include "./helicopter.h"
#include "./player.h"

// Owns everything in a play session: entities, waves, scoring and the HUD
class World {
 public:
  void reset(int high_score);

  void update(float dt);

  // World layer, drawn to the shaking canvas
  void draw() const;

  // Overlay that does not shake
  void draw_hud() const;

  Player& get_player() { return player; }
  Effects& get_effects() { return effects; }
  const Effects& get_effects() const { return effects; }

  void spawn_bullet(const Bullet& bullet);
  void spawn_mine(const asw::Vec2f& pos, const asw::Vec2f& vel);
  void spawn_pickup(const asw::Vec2f& pos, PickupType type);

  // Continuous laser damage along a ray
  void laser_sweep(const asw::Vec2f& origin,
                   const asw::Vec2f& dir,
                   float length,
                   float damage);

  void on_player_damaged();
  void on_player_death();

  bool is_game_over() const { return game_over; }
  int get_score() const { return score; }
  int get_wave() const { return wave; }

 private:
  enum class Phase {
    Intro,
    Fighting,
    Clear,
  };

  void start_wave(int number);
  void update_waves(float dt);
  void update_collisions();

  void kill_helicopter(Helicopter& heli);
  void explode_mine(Mine& mine, bool by_player);
  void drop_pickups(const Helicopter& heli);
  void clear_enemy_bullets();

  void add_score(int points);
  int get_multiplier() const;

  void draw_banner() const;

  Background background;
  Player player;
  Effects effects;

  std::vector<Helicopter> helicopters;
  std::vector<Bullet> bullets;
  std::vector<Mine> mines;
  std::vector<Pickup> pickups;

  // Waves
  int wave{0};
  Phase phase{Phase::Intro};
  float phase_timer{0.0F};
  std::vector<HeliType> spawn_queue;
  float spawn_timer{0.0F};
  float spawn_interval{1.5F};
  size_t max_alive{2};
  float escort_timer{0.0F};
  bool perfect_wave{true};
  std::string banner;
  std::string sub_banner;
  bool banner_warning{false};

  // Score
  int score{0};
  int high_score{0};
  int next_extra_life{0};
  int combo{0};
  float combo_timer{0.0F};
  int lives{3};

  // Death and hit stop
  float respawn_timer{0.0F};
  float hit_stop{0.0F};
  bool game_over{false};
  float time{0.0F};
};
