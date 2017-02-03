#ifndef ENEMY_H
#define ENEMY_H


#include "tools.h"
#include "world.h"

class world;

class enemy{
  public:
    enemy(BITMAP* newHelicopterSprite, BITMAP* newHelicopterHurt, world *newGameWorld);
    ~enemy();
    void update(int player_x, int player_y);
    void draw(BITMAP *tempBitmap);
  private:
    world *game_world;
    int x;
    int y;//30
    int direction;//LEFT
    int hurt_timer;
    float health;//100
    int fire_rate;//20
    int fire_timer;
    int movement_timer;
    float angle_radians;

    BITMAP* helicopter_sprite;
    BITMAP* helicopter_hurt;


};

#endif
