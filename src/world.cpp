#include "world.h"

world::world(){





}

world::~world(){

}

void world::setup(){

  if (!(helicopter_sprite = load_bitmap("helicopter.png", NULL)))
    abort_on_error("Cannot find image helicopter.png\nPlease check your files and try again");

  if (!(helicopter_hurt = load_bitmap("helicopter_hurt.png", NULL)))
    abort_on_error("Cannot find image helicopter_hurt.png\nPlease check your files and try again");

  if (!(character_sprite = load_bitmap("player.png", NULL)))
    abort_on_error("Cannot find image player.png\nPlease check your files and try again");

  if (!(character_hurt = load_bitmap("player_hurt.png", NULL)))
    abort_on_error("Cannot find image player_hurt.png\nPlease check your files and try again");

  if (!(background = load_bitmap("background.png", NULL)))
    abort_on_error("Cannot find image background.png\nPlease check your files and try again");

  create_helicopter();
  game_character.setup(character_sprite,character_hurt);


}
//Helicopter factory
void world::create_helicopter(){
   game_enemies.push_back(enemy(helicopter_sprite,helicopter_hurt));

}

//Bullet factory
void world::create_projectile(int newX, int newY, bool newOwner, float newAngle, float newSpeed){
  game_projectiles.push_back(projectile(newX,newY,newOwner,newAngle,newSpeed));
  projectile_delay=0;
}

void world::update(){

  game_character.update();

  for(int i=0; i<game_enemies.size(); i++){
    game_enemies[i].update(0,0);
  }

  for(int i=0; i<game_projectiles.size(); i++){
    game_projectiles[i].update();
  }

  projectile_delay++;

}

void world::draw(BITMAP *tempBitmap){

  draw_sprite(tempBitmap,background,0,0);
  game_character.draw(tempBitmap);


  for(int i=0; i<game_projectiles.size(); i++){
    game_projectiles[i].draw(tempBitmap);
  }

  for(int i=0; i<game_enemies.size(); i++){
    game_enemies[i].draw(tempBitmap);
  }



}
