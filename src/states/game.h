#pragma once

#include <asw/asw.h>

#include "../game/world.h"
#include "./state.h"

class Game : public asw::scene::Scene<ProgramState> {
 public:
  using asw::scene::Scene<ProgramState>::Scene;

  void init() override;
  void update(float dt) override;
  void draw() override;

 private:
  void restart();
  void draw_game_over() const;
  void draw_paused() const;

  World world;

  // World layer, drawn offset for screen shake
  asw::Texture canvas;

  bool paused{false};
  float game_over_time{0.0F};
  bool new_high_score{false};
  int high_score{0};
  float time{0.0F};
};
