#pragma once

#include <asw/asw.h>

#include <vector>

#include "../game/background.h"
#include "./state.h"

class Menu : public asw::scene::Scene<ProgramState> {
 public:
  using asw::scene::Scene<ProgramState>::Scene;

  void init() override;
  void update(float dt) override;
  void draw() override;

 private:
  // Helicopters flying past behind the title
  struct Flyby {
    asw::Vec2f pos;
    float speed;
    float scale;
    asw::Color tint;
  };

  void add_flyby();

  Background background;
  std::vector<Flyby> flybys;
  float time{0.0F};
  float flyby_timer{0.0F};
  int high_score{0};
};
