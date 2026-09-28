#pragma once

#include <asw/asw.h>

#include <vector>

// Static backdrop with drifting clouds, shared by the menu and the game
class Background {
 public:
  Background();

  void update(float dt);
  void draw() const;

 private:
  struct Cloud {
    asw::Vec2f pos;
    float speed;
    float scale;
  };

  std::vector<Cloud> clouds;
};
