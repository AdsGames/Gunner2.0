#include "character.h"

character::character(){

}

character::~character(){


}
void character::setup(BITMAP* newCharacterSprite, BITMAP* newCharacterHurt, world *newGameWorld){
  character_sprite=newCharacterSprite;
  character_hurt=newCharacterHurt;
  game_world = newGameWorld;
  y=550;
  health=100;
  jump_timer=100;

}
int character::get_x(){
  return x;
}

int character::get_y(){
  return y;
}

void character::update(){

  mouse_angle_radians=find_angle(x+15,y+20,mouse_x,mouse_y);

  if((key[KEY_LEFT] || key[KEY_A]) && x>1)x-=10;
  if((key[KEY_RIGHT] || key[KEY_D]) && x<750)x+=10;

  jump_timer++;
  projectile_delay++;

  if((key[KEY_SPACE]||key[KEY_W]) && jump_timer>20){
    jump_timer=0;
  }

  if(jump_timer > 0 && 10 > jump_timer){
    y-=20;
  }
  if(jump_timer>10 && jump_timer<20){
    y+=20;
  }


  if((mouse_b & 1) && projectile_delay>10 ){
    game_world -> create_projectile(x+15,y+20,PLAYER,mouse_angle_radians,5);
    projectile_delay=0;
  }

}

void character::draw(BITMAP *tempBitmap){

  if(hurt_timer<1)draw_sprite(tempBitmap,character_sprite,x,y);
  else draw_sprite(tempBitmap,character_hurt,x,y);

  rectfill(tempBitmap,550,10,754,30,makecol(0,0,0));
  rectfill(tempBitmap,552,12,752,28,makecol(255,0,0));
  rectfill(tempBitmap,552,12,552+(health*2),28,makecol(0,255,0));


}
