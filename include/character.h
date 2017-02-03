#ifndef CHARACTER_H
#define CHARACTER_H

#include "tools.h"

class character{
  public:
    character();
    ~character();
    void draw(BITMAP *tempBitmap);
    void update();
    void setup(BITMAP *newCharacterSprite, BITMAP *newCharacterHurt);
    int get_x();
    int get_y();
  private:

    BITMAP* character_sprite;
    BITMAP* character_hurt;
    int jump_timer;
    int x;
    int y;
    int hurt_timer;
    int health;
    int fire_rate;
    int fire_delay_rate;
    int fire_rate_timer;
    int laser_timer;
    int bouncy_timer;
    bool is_lasering;
    float mouse_angle_radians;
    float mouse_angle_allegro;


};
#endif
