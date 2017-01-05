#include "enemy.h"

enemy::enemy(BITMAP* newHelicopterSprite, BITMAP* newHelicopterHurt){

  helicopter_sprite=newHelicopterSprite;
  helicopter_hurt=newHelicopterHurt;

  fire_rate=20;
  health=100;
  x=random(1,600);
  y=random(1,200);
  direction=LEFT;
}

enemy::~enemy(){


}

void enemy::update(int player_x,int player_y){

  //if(random(1,1000)==1)create_mine(x,y);

            angle_radians=find_angle(x+100,y+30,player_x+15,player_y+20);

            if(direction==LEFT)x-=5;
            if(direction==RIGHT)x+=5;



            if(x>600){
                direction=HOVER;
                movement_timer++;
                if(movement_timer>120){
                    direction=LEFT;
                    movement_timer=0;
                }
            }
            if(x<0){
                direction=HOVER;
                movement_timer++;
                if(movement_timer>120){
                    direction=RIGHT;
                    movement_timer=0;
                }
            }
            if(fire_rate<fire_timer){
                //create_bullet(x+100,y+30,HELICOPTER,angle_radians,7);
                fire_timer=0;
            }
             hurt_timer--;
            fire_timer++;


              /*
             if(health<1){


                if(helicopter_killcount<15){
                    if(random(1,15-helicopter_killcount)==1){
                        create_box(x,y,0);
                    }
                }else if(random(1,2)==1)create_box(x-76,y,0);

                if(random(1,2)==1)create_box(x,y,3);

                if(helicopter_killcount<20){
                    if(random(1,20-helicopter_killcount)==1){
                        create_box(x,y,2);
                    }
                }else create_box(x-76,y,2);




                create_box(x+76,y,1);

                health=100;
                if(helicopter_killcount<10)create_helicopter(1);
                if(helicopter_killcount>9)create_helicopter(random(1,2));

                helicopter_killcount++;


            }*/

        }



void enemy::draw(BITMAP *tempBitmap){

  rectfill(tempBitmap,x,y-10,x+102,y+2,makecol(0,0,0));
  rectfill(tempBitmap,x+2,y-8,x+100,y,makecol(255,0,0));
  rectfill(tempBitmap,x+2,y-8,x+(health),y,makecol(0,255,0));

 // if(hurt_timer<1)draw_sprite(tempBitmap,helicopter_sprite,x,y);
  //if(hurt_timer>0)draw_sprite(tempBitmap,helicopter_hurt,x,y);


}
