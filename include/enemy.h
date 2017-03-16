#ifndef ENEMY_H
#define ENEMY_H


#include "tools.h"
#include "world.h"

class world;

class enemy{
  public:
    enemy(BITMAP* newHelicopterSprite, BITMAP* newHelicopterHurt, world *newGameWorld);
    ~enemy();
    void update();
    void draw(BITMAP *tempBitmap);
  private:

    world *game_world;

    int width = 0;
    int height = 0;

    int x = 25;
    int y = 30;
    int direction = LEFT;
    int hurt_timer = 0;
    float health = 100;
    int fire_rate = 20;
    int fire_timer = 0;
    int movement_timer = 0;
    float angle_radians = 0;

    BITMAP *enemy_sprite;
    BITMAP *enemy_hurt;


};

#endif
