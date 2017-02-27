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
    int x=25;
    int y=30;//30
    int direction=LEFT;//LEFT
    int hurt_timer=0;
    float health=100;//100
    int fire_rate=20;//20
    int fire_timer=0;
    int movement_timer=0;
    float angle_radians=0;

    BITMAP* helicopter_sprite;
    BITMAP* helicopter_hurt;


};

#endif
