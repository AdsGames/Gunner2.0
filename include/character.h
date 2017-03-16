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
    void setup(BITMAP *newCharacterSprite, BITMAP *newCharacterHurt, BITMAP* newIcon, world *newGameWorld);
    int get_x();
    int get_y();
    float radians( float degrees );
    void draw_timer(BITMAP *bitmap, int newX, int newY, int newTime,float newRadius);


  private:

    world *game_world;

    BITMAP *character_sprite;
    BITMAP *character_hurt;
    BITMAP *icon;
    int jump_timer=0;

    int x=0;
    int y=0;
    int hurt_timer=0;
    int health;

    int width=0;
    int height=0;

    int rapidfire_timer;
    int ricochet_timer;


    int weapon=0;
    //int fire_rate[5];
    //bool fire_mode[5]={false,false,false,false,false};
    int poop[5];


    int time=0;

    int projectile_delay=0;


    bool weapon_fired=false;

    float mouse_angle_radians;


};
#endif
