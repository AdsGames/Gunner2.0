#ifndef CHARACTER_H
#define CHARACTER_H

#include "tools.h"
#include "world.h"

class world;

class character{
  public:
    character();
    ~character();
    void draw(BITMAP *tempBitmap);
    void update();
    void setup(BITMAP *newCharacterSprite, BITMAP *newCharacterHurt, world *newGameWorld);
    int get_x();
    int get_y();

  private:

    world *game_world;

    BITMAP *character_sprite;
    BITMAP *character_hurt;
    int jump_timer;
    int x;
    int y;
    int hurt_timer;
    int health;

    int projectile_delay;

    float mouse_angle_radians;


};
#endif
