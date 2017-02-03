#ifndef WORLD_H
#define WORLD_H

#include "tools.h"
#include "projectile.h"
#include "character.h"
#include "item.h"

class enemy;

class world{
  public:
    world();
    ~world();
    void update();
    void setup();
    void draw(BITMAP *tempBitmap);
    void create_helicopter();
    void create_projectile(int newX, int newY, bool newOwner, float newAngle, float newSpeed);
    void create_item(int newType, int newX, int newY);
  private:

    character game_character;
    BITMAP *background;
    BITMAP *cursor;

    BITMAP *character_sprite;
    BITMAP *character_hurt;

    BITMAP *helicopter_sprite;
    BITMAP *helicopter_hurt;

    BITMAP *crate;


    int projectile_delay;

    std::vector<projectile> game_projectiles;
    std::vector<enemy> game_enemies;
    std::vector<item> game_items;






};








#endif
