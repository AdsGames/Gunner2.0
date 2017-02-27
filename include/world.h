#ifndef WORLD_H
#define WORLD_H

#include <algorithm>    // std::find

#include "tools.h"
#include "projectile.h"
#include "item.h"
#include "character.h"
#include "enemy.h"

class enemy;
class character;

class world{
  public:
    world();
    ~world();
    void update();
    void setup();
    void draw(BITMAP *tempBitmap);

    void create_enemy();
    void delete_enemy(enemy* newEnemy);

    std::vector<projectile*>* get_projectiles();
    void create_projectile(int newX, int newY, bool newOwner, float newAngle, float newSpeed);
    void delete_projectile(projectile* newProjectile);

    std::vector<item*>* get_items();
    void create_item(int newType, int newX, int newY);
    void delete_item(item* newItem);



    int get_character_x();
    int get_character_y();

  private:

    int enemy_spawn_delay=301;

    character *game_character;
    BITMAP *background;
    BITMAP *cursor;

    BITMAP *character_sprite;
    BITMAP *character_hurt;

    BITMAP *helicopter_sprite;
    BITMAP *helicopter_hurt;

    BITMAP *crate;



    std::vector<projectile*> game_projectiles;
    std::vector<enemy*> game_enemies;
    std::vector<item*> game_items;






};








#endif
