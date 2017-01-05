#ifndef WORLD_H
#define WORLD_H

#include "tools.h"

class world{
  public:
    world();
    ~world();
    void update();
    void setup();
    void draw(BITMAP *tempBitmap);
    void create_helicopter();
    void create_projectile(int newX, int newY, bool newOwner, float newAngle, float newSpeed);
  private:

    character game_character;
    BITMAP *background;

    BITMAP *character_sprite;
    BITMAP *character_hurt;

    BITMAP *helicopter_sprite;
    BITMAP *helicopter_hurt;


    int projectile_delay;

    std::vector<projectile> game_projectiles;
    std::vector<enemy> game_enemies;






};








#endif
