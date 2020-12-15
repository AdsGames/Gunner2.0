#include "world.h"

world::world(){


}

world::~world(){

}

void world::setup(){

  if (!(icon = load_png("icon.png", NULL)))
    abort_on_error("Cannot find image icon.png\nPlease check your files and try again");

  if (!(crate[EMPTY] = load_png("crate.png", NULL)))
    abort_on_error("Cannot find image crate.png\nPlease check your files and try again");

  if (!(crate[HEALTH] = load_png("crate_health.png", NULL)))
    abort_on_error("Cannot find image crate_health.png\nPlease check your files and try again");

  if (!(crate[RAPIDFIRE]= load_png("crate_rapidfire.png", NULL)))
    abort_on_error("Cannot find image crate_rapidfire.png\nPlease check your files and try again");

  if (!(crate[RICOCHET] = load_png("crate_ricochet.png", NULL)))
    abort_on_error("Cannot find image crate_ricochet.png\nPlease check your files and try again");

  if (!(cursor = load_png("cursor.png", NULL)))
    abort_on_error("Cannot find image cursor.png\nPlease check your files and try again");

  if (!(helicopter_sprite = load_png("helicopter.png", NULL)))
    abort_on_error("Cannot find image helicopter.png\nPlease check your files and try again");

  if (!(helicopter_hurt = load_png("helicopter_hurt.png", NULL)))
    abort_on_error("Cannot find image helicopter_hurt.png\nPlease check your files and try again");

  if (!(character_sprite = load_png("player.png", NULL)))
    abort_on_error("Cannot find image player.png\nPlease check your files and try again");

  if (!(character_hurt = load_png("player_hurt.png", NULL)))
    abort_on_error("Cannot find image player_hurt.png\nPlease check your files and try again");

  if (!(background = load_png("background.png", NULL)))
    abort_on_error("Cannot find image background.png\nPlease check your files and try again");

  create_enemy();
  game_character = new character();
  game_character -> setup( character_sprite, character_hurt, icon, this);

}

//Helicopter factory
void world::create_enemy(){
  enemy *newEnemy = new enemy( helicopter_sprite, helicopter_hurt, this);
  game_enemies.push_back( newEnemy);
}

void world::kill_player(){
  character_alive=false;
}

void world::delete_enemy(enemy* newEnemy){

  std::vector<enemy*>::iterator i;
  i=std::find(game_enemies.begin(),game_enemies.end(),newEnemy);

  if(i!=game_enemies.end()){
      game_enemies.erase(i);
  }
  enemy_spawn_delay=0;
}

void world::delete_projectile(projectile* newProjectile){

  std::vector<projectile*>::iterator i;
  i=std::find(game_projectiles.begin(),game_projectiles.end(),newProjectile);

  if(i!=game_projectiles.end()){
      game_projectiles.erase(i);
  }
}


void world::create_item(int newType, int newX, int newY){

  game_items.push_back(new item(crate[newType],newX,newY,newType));

}
void world::delete_item(item* newItem){

  std::vector<item*>::iterator i;
  i=std::find(game_items.begin(),game_items.end(),newItem);

  if(i!=game_items.end()){
      game_items.erase(i);
  }
}
int world::get_character_x(){
  return game_character -> get_x();
}

int world::get_character_y(){
  return game_character -> get_y();
}

//Bullet factory
void world::create_projectile(int newX, int newY, bool newOwner, float newAngle, float newSpeed, bool newRicochet, int newWidth, int newHeight){
  game_projectiles.push_back(new projectile(newX,newY,newOwner,newAngle,newSpeed,newRicochet,newWidth,newHeight));
}

std::vector<projectile*>* world::get_projectiles(){

  return &game_projectiles;

}

std::vector<item*>* world::get_items(){

  return &game_items;

}


void world::update(){

  if(enemy_spawn_delay==300){
    create_enemy();
    enemy_spawn_delay++;
  }else
    enemy_spawn_delay++;

  if(character_alive)
    game_character -> update();

  for( unsigned int i=0; i<game_enemies.size(); i++){
    game_enemies[i] -> update();
  }

  for( unsigned int i=0; i<game_projectiles.size(); i++){
    game_projectiles[i] -> update();
    if(game_projectiles[i] -> get_lifetime()<0)
      delete_projectile(game_projectiles[i]);
  }

  for( unsigned int i=0; i<game_items.size(); i++){
    game_items[i] -> update();
  }

}

void world::draw(BITMAP *tempBitmap){

  draw_sprite(tempBitmap,background,0,0);

  if(character_alive)
    game_character -> draw(tempBitmap);


  for( unsigned int i = 0; i < game_projectiles.size(); i++){
    game_projectiles[i] -> draw(tempBitmap);
  }

  for( unsigned int i = 0; i < game_enemies.size(); i++){
    game_enemies[i] -> draw(tempBitmap);
  }

  for( unsigned int i = 0; i < game_items.size(); i++){
    game_items[i] -> draw(tempBitmap);
  }

  draw_sprite( tempBitmap, cursor, mouse_x - 10, mouse_y - 10);
}
