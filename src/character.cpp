#include "character.h"

character::character(){

}

character::~character(){


}
void character::setup(BITMAP* newCharacterSprite, BITMAP* newCharacterHurt, BITMAP *newIcon, world *newGameWorld){

  character_sprite=newCharacterSprite;
  character_hurt=newCharacterHurt;
  icon = newIcon;

  game_world = newGameWorld;

  width = character_sprite -> w;
  height = character_sprite -> h;

  y=SCREEN_H-50;
  health=100;
  jump_timer=100;
  projectile_delay=0;
  fire_rate=10;


}
int character::get_x(){
  return x;
}

int character::get_y(){
  return y;
}

void character::draw_timer(BITMAP *bitmap, int newX, int newY, int newTime, float newRadius){

  int centre_x = newX+newRadius;
  int centre_y = newY+newRadius;
  circlefill(bitmap, newX+newRadius,newY+newRadius,newRadius-1,makecol(255,0,0));

  int points[720];

  points[0]=centre_x;
  points[1]=centre_y;

  points[2]=centre_x;
  points[3]=centre_y-newRadius;

  for(int i=0; i<time; i+=2){

    points [i+4] = centre_x + (float)newRadius*cos(degrees_to_radians(i)-PI/2);
    points [i+5] = centre_y + (float)newRadius*sin(degrees_to_radians(i)-PI/2 );
  }

  polygon(bitmap, time/2, points, makecol(0, 0, 0));

}

void character::update(){

   for( unsigned int i = 0; i < game_world -> get_projectiles() -> size(); i++){
    if( collision(x,
                  x + 40, game_world -> get_projectiles() -> at(i) -> get_x(),
                  game_world -> get_projectiles() -> at(i) -> get_x() + game_world -> get_projectiles() -> at(i) -> get_width(),
                  y,
                  y + 40,
                  game_world -> get_projectiles() -> at(i) -> get_y(),
                  game_world -> get_projectiles() -> at(i) -> get_y() + game_world -> get_projectiles() -> at(i) -> get_height())
                  && !game_world -> get_projectiles() -> at(i) -> get_owner()
                  )
    {
      health -= 5;
      hurt_timer = 3;
      game_world -> delete_projectile(game_world -> get_projectiles() -> at(i));

    }
  }

  for( unsigned int i = 0; i < game_world -> get_items() -> size(); i++){
    if( collision(x,
                  x + 40,
                  game_world -> get_items() -> at(i) -> get_x(),
                  game_world -> get_items() -> at(i) -> get_x() + game_world -> get_items() -> at(i) -> get_width(),
                  y,
                  y + 40,
                  game_world -> get_items() -> at(i) -> get_y(),
                  game_world -> get_items() -> at(i) -> get_y() + game_world -> get_items() -> at(i) -> get_height())
                 )
    {
      if(game_world -> get_items() -> at(i) -> get_type() == HEALTH){
        health+=10;
      }
      if(game_world -> get_items() -> at(i) -> get_type() == RAPIDFIRE){
        rapidfire_timer=1000;
        fire_rate=2;
      }
      if(game_world -> get_items() -> at(i) -> get_type() == RICOCHET){
        ricochet_timer=1000;

      }


      game_world -> delete_item(game_world -> get_items() -> at(i));

    }
  }

  mouse_angle_radians = find_angle(x+15,y+20,mouse_x,mouse_y);

  if((key[KEY_LEFT] || key[KEY_A]) && x>1)x-=10;
  if((key[KEY_RIGHT] || key[KEY_D]) && x<750)x+=10;

  jump_timer++;
  projectile_delay++;
  hurt_timer--;
  rapidfire_timer--;
  ricochet_timer--;


  if(rapidfire_timer<=0){
    fire_rate=10;
    rapidfire_timer=0;
  }

  if(time>360)
    time=0;
  time+=6;

  if(ricochet_timer<=0)
    ricochet_timer=0;

  if((key[KEY_SPACE]||key[KEY_W]) && jump_timer>20){
    jump_timer=0;
  }

  if(jump_timer > 0 && 10 > jump_timer){
    y-=20;
  }
  if(jump_timer>10 && jump_timer<20){
    y+=20;
  }

  if((mouse_b & 1) && projectile_delay>fire_rate){

    game_world -> create_projectile(x+15,y+20,PLAYER,mouse_angle_radians,5,ricochet_timer>0,5,5);
    projectile_delay=0;
  }
  if(health<=0){
    health=0;
    game_world -> kill_player();
  }
  if(health>100){
    health=100;
  }

}

void character::draw(BITMAP *tempBitmap){

  if(hurt_timer<1)draw_sprite(tempBitmap,character_sprite,x,y);
  else draw_sprite(tempBitmap,character_hurt,x,y);

  rectfill(tempBitmap,SCREEN_W-250,10,SCREEN_W-46,30,makecol(0,0,0));
  rectfill(tempBitmap,SCREEN_W-248,12,SCREEN_W-48,28,makecol(255,0,0));
  rectfill(tempBitmap,SCREEN_W-248,12,(SCREEN_W-248)+(health*2),28,makecol(0,255,0));
  //draw_sprite(tempBitmap,icon,0,0);

  draw_timer(tempBitmap,75,200,time,50);

  draw_timer(tempBitmap,200,200,time,75);

   draw_timer(tempBitmap,100,100,time,25);

}
