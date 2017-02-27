#include "projectile.h"

projectile::projectile( float newX, float newY, bool newOwner, float newAngle, float newSpeed, bool newRicochet, int newWidth, int newHeight){

  x = newX;
  y = newY;
  x_velocity = -newSpeed * cos(newAngle);
  y_velocity = -newSpeed * sin(newAngle);
  owner = newOwner;
  width = newWidth;
  height = newHeight;
  ricochet = newRicochet;
  lifetime=500;

}
projectile::~projectile(){

}

float projectile::get_x(){
  return this -> x;
}

float projectile::get_y(){
  return this -> y;
}

int projectile::get_width(){
  return this -> width;
}

int projectile::get_height(){
  return this -> height;
}

int projectile::get_lifetime(){
  return lifetime;
}

void projectile::update(){

  lifetime--;

  x+=x_velocity;
  y+=y_velocity;

  if(ricochet){
    if(x+width>SCREEN_W)
      x_velocity=-x_velocity;
    if(x<0)
      x_velocity=-x_velocity;

    if(y+height>SCREEN_H)
      y_velocity=-y_velocity;
    if(y<0)
      y_velocity=-y_velocity;
  }

            /*
            for(int j=0; j<10; j++){
                if(helicopter[j].on_screen){
                    if(collision(helicopter[j].x,helicopter[j].x+200,bullets[i].x,bullets[i].x+5,helicopter[j].y,helicopter[j].y+40,bullets[i].y,bullets[i].y+5) && bullets[i].on_screen && bullets[i].owner){
                        helicopter[j].health-=5;
                        bullets[i].on_screen=false;
                        helicopter[j].hurt_timer=3;
                    }
                }
            }
            for(int j=0; j<10; j++){
                if(mine[j].on_screen){
                    if(collision(mine[j].x,mine[j].x+200,bullets[i].x,bullets[i].x+5,mine[j].y-50,mine[j].y+40,bullets[i].y,bullets[i].y+5) && bullets[i].on_screen && bullets[i].owner){
                        bullets[i].on_screen=false;
                        mine[j].health--;
                    }
                }
            }
            if(collision(player_x,player_x+50,bullets[i].x,bullets[i].x+5,player_y,player_y+50,bullets[i].y,bullets[i].y+5) && !bullets[i].owner){
                player_hurt_timer=3;
                bullets[i].on_screen=false;
                player_health-=5;
            }
            if(bullets[i].owner==PLAYER){
            if(bullets[i].x>800){
             if(player_bouncy_timer<0)
                bullets[i].on_screen=false;
             if(player_bouncy_timer>0){
                bullets[i].vector_x=-bullets[i].vector_x;

             }
           }


           if(bullets[i].x<0){
             if(player_bouncy_timer<0)
                bullets[i].on_screen=false;
             if(player_bouncy_timer>0){
                bullets[i].vector_x=-bullets[i].vector_x;

             }
           }

           if(bullets[i].y>600){
             if(player_bouncy_timer<0)
                bullets[i].on_screen=false;
             if(player_bouncy_timer>0){

                bullets[i].vector_y=-bullets[i].vector_y;
             }
           }


          if(bullets[i].y<0){
             if(player_bouncy_timer<0)
                bullets[i].on_screen=false;
             if(player_bouncy_timer>0){

                bullets[i].vector_y=-bullets[i].vector_y;
             }
           }
        }

            if(bullets[i].owner==HELICOPTER)if(bullets[i].x>800 || bullets[i].y>600 || bullets[i].x<0 || bullets[i].y<0)bullets[i].on_screen=false;
        */
}

void projectile::draw(BITMAP *tempBitmap){

  if(owner){
    rectfill(tempBitmap,x,y,x+width,y+height,makecol(0,0,0));

  }else{
    rectfill(tempBitmap,x,y,x+width,y+width,makecol(255,0,0));

  }


}
