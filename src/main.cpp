#include "tools.h"
#include "item.h"
#include "world.h"


#define PLAYER TRUE
#define HELICOPTER FALSE

#define LEFT 0
#define RIGHT 1
#define HOVER 2

BITMAP* buffer;
BITMAP* cursor;
BITMAP* box_machinegun;
BITMAP* box_health;
BITMAP* laserbeam;
BITMAP* box_laserbeam;
BITMAP* box_bouncy;
BITMAP* mine_image;

bool close_button_pressed;

// FPS System
volatile int ticks = 0;
const int updates_per_second = 60;
volatile int game_time = 0;

int fps;
int frames_done;
int old_time;

world game_world;

void ticker(){
  ticks++;
}
END_OF_FUNCTION(ticker)

void game_time_ticker(){
  game_time++;
}
END_OF_FUNCTION(ticker)

void close_button_handler(void){
  close_button_pressed = TRUE;
}
END_OF_FUNCTION(close_button_handler)

//Collision between 2 boxes
bool collision(float xMin1, float xMax1, float xMin2, float xMax2, float yMin1, float yMax1, float yMin2, float yMax2)
{
  if (xMin1 < xMax2 && yMin1 < yMax2 && xMin2 < xMax1 && yMin2 < yMax1){
    return true;
  }
  return false;
}

//Mine factory
/*
void create_mine(int newX, int newY){
    bool mine_made=false;
    for(int i=0; i<100; i++){
        if(!mine[i].on_screen && !mine_made){
            mine_made=true;
            mine[i].on_screen=true;
            mine[i].x=newX;
            mine[i].y=newY;
            mine[i].health=5;

        }

    }

}

*/

//Raytracer
void raytrace(){
    /*
    player_is_lasering=true;
    int i=1;
    raytracer[i].x=player_x;
    raytracer[i].y=player_y;
    raytracer[i].vector_x=-1*cos(mouse_angle_radians);
    raytracer[i].vector_y=-1*sin(mouse_angle_radians);
    while(raytracer[i].x<800 && raytracer[i].y<600 && raytracer[i].x>0 && raytracer[i].y>0){
        raytracer[i].x+=raytracer[i].vector_x;
        raytracer[i].y+=raytracer[i].vector_y;
        for(int j=0; j<10; j++){
            if(collision(helicopter[j].x,helicopter[j].x+200,raytracer[i].x,raytracer[i].x,helicopter[j].y,helicopter[j].y+40,raytracer[i].y,raytracer[i].y)){
                if(helicopter[j].on_screen){
                    helicopter[j].health-=0.05;
                    helicopter[j].hurt_timer=3;
                }
           }
        }

    }*/

}
/*
//Box factory
void create_box(int newX, int newY, int newType){
    bool box_made=false;
    for(int i=0; i<100; i++){
        if(!box[i].on_screen && !box_made){
            box_made=true;
            box[i].on_screen=true;
            box[i].x=newX;
            box[i].y=newY;
            box[i].type=newType;
        }
    }
    //bullet_delay=0;
}
*/




void update(){

  game_world.update();





/*
    for(int i=0; i<100; i++){
        if(mine[i].on_screen){
            if(mine[i].y<570)mine[i].y+=10;
            if(collision(mine[i].x,mine[i].x+45,player_x,player_x+50,mine[i].y,mine[i].y+20,player_y,player_y+45)){
              mine[i].on_screen=false;
              player_health-=20;
            }
        }if(mine[i].health<1)
          mine[i].on_screen=false;

    }
*/










   // if((mouse_b & 1) && bullet_delay>player_fire_delay_rate ){
   //     if(player_laser_timer<1)game_world.create_projectile(player_x+15,player_y+20,PLAYER,mouse_angle_radians,player_fire_rate);
   //     else raytrace();
   // }
    /*
    for(int i=0; i<10; i++){
        if(box[i].on_screen){
            if(box[i].y<550)box[i].y+=5;
             if(collision(player_x,player_x+50,box[i].x,box[i].x+75,player_y,player_y+50,box[i].y,box[i].y+50)){
                box[i].on_screen=false;
                if(box[i].type==2){
                    player_laser_timer=120;
                }
                if(box[i].type==1){
                    if(player_health<90)player_health+=10;
                    else player_health=100;
                }
                if(box[i].type==0){
                    player_fire_rate=20;
                    player_fire_delay_rate=3;
                    player_fire_rate_timer=600;
                }
                if(box[i].type==3){
                    player_bouncy_timer=600;
                }
             }


        }
        */
    //}






}

void draw(){

   game_world.draw(buffer);

  /*
    for(int i=0; i<10; i++){
        if(box[i].on_screen){

            if(box[i].type==0)draw_sprite(buffer,box_machinegun,box[i].x,box[i].y);
            if(box[i].type==1)draw_sprite(buffer,box_health,box[i].x,box[i].y);
            if(box[i].type==2)draw_sprite(buffer,box_laserbeam,box[i].x,box[i].y);
            if(box[i].type==3)draw_sprite(buffer,box_bouncy,box[i].x,box[i].y);


        }
    }
    for(int i=0; i<100; i++){
        if(mine[i].on_screen){
            draw_sprite(buffer,mine_image,mine[i].x,mine[i].y);
            rectfill(buffer,mine[i].x+20,mine[i].y-16,mine[i].x+42,mine[i].y-10,makecol(0,0,0));
            rectfill(buffer,mine[i].x+21,mine[i].y-15,mine[i].x+21+(5*4),mine[i].y-11,makecol(255,0,0));
            rectfill(buffer,mine[i].x+21,mine[i].y-15,mine[i].x+21+(mine[i].health*4),mine[i].y-11,makecol(0,255,0));

        }
    }
    //if(player_is_lasering)rotate_sprite(buffer,laserbeam,player_x-780,player_y,itofix(mouse_angle_allegro));
    //player_is_lasering=false;
    textprintf_ex(buffer,font,20,20,makecol(0,0,0),-1,"Helicopter Killcount: %i",helicopter_killcount);
*/
    draw_sprite(screen,buffer,0,0);


}







void setup(){

    game_world.setup();

    buffer=create_bitmap(800,600);






    srand(time(NULL));

     // Setup for FPS system
    LOCK_VARIABLE(ticks);
    LOCK_FUNCTION(ticker);
    install_int_ex(ticker, BPS_TO_TIMER(updates_per_second));

    LOCK_VARIABLE(game_time);
    LOCK_FUNCTION(game_time_ticker);
    install_int_ex(game_time_ticker, BPS_TO_TIMER(10));

    // Close button
    LOCK_FUNCTION(close_button_handler);
    set_close_button_callback(close_button_handler);






    if (!(box_machinegun = load_bitmap("box_machinegun.png", NULL)))
      abort_on_error("Cannot find image box_machinegun.png\nPlease check your files and try again");

    if (!(box_health = load_bitmap("box_health.png", NULL)))
      abort_on_error("Cannot find image box_health.png\nPlease check your files and try again");

    if (!(box_bouncy = load_bitmap("box_bouncy.png", NULL)))
      abort_on_error("Cannot find image box_bouncy.png\nPlease check your files and try again");

    if (!(laserbeam = load_bitmap("laserbeam.png", NULL)))
      abort_on_error("Cannot find image laserbeam.png\nPlease check your files and try again");

    if (!(box_laserbeam = load_bitmap("box_laserbeam.png", NULL)))
      abort_on_error("Cannot find image box_laserbeam.png\nPlease check your files and try again");

    if (!(mine_image = load_bitmap("mine.png", NULL)))
      abort_on_error("Cannot find image mine.png\nPlease check your files and try again");



}






int main(){

  allegro_init();
  alpng_init();
  install_timer();
  install_keyboard();
  install_mouse();
  set_color_depth(32);


  set_gfx_mode(GFX_AUTODETECT_WINDOWED,800,600, 0, 0);
  install_sound(DIGI_AUTODETECT,MIDI_AUTODETECT,".");

  set_window_title("Gunner 2.0");
  setup();


    while(!key[KEY_ESC] && !close_button_pressed){
        while(ticks == 0){
            rest(1);
        }
    while(ticks > 0){
        int old_ticks = ticks;

        update();

        ticks--;
        if(old_ticks <= ticks){
            break;
        }
    }
        if(game_time - old_time >= 10){
            fps = frames_done;
            frames_done = 0;
            old_time = game_time;
        }
        draw();
    }


	return 0;
}
END_OF_MAIN()
