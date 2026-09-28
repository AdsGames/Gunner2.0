#pragma once

#include <asw/asw.h>

#include "./state.h"

// Loads every asset once, then moves on to the menu
class Init : public asw::scene::Scene<ProgramState> {
 public:
  using asw::scene::Scene<ProgramState>::Scene;

  void init() override;
  void update(float dt) override;
  void draw() override;
};
