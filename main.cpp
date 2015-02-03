#include<allegro.h>
#include<alpng.h>
#include<time.h>
#include<cmath>

#define PLAYER TRUE
#define HELICOPTER FALSE

#define LEFT 0
#define RIGHT 1
#define HOVER 2

BITMAP* buffer;
BITMAP* player;
BITMAP* player_hurt;
BITMAP* background;
BITMAP* cursor;
BITMAP* bullet_image;
BITMAP* helicopter_bullet_image;
BITMAP* helicopter_image;
BITMAP* helicopter_hurt;
BITMAP* box_machinegun;
BITMAP* box_health;
BITMAP* laserbeam;
BITMAP* box_laserbeam;
BITMAP* mine_image;

bool close_button_pressed;

// FPS System
volatile int ticks = 0;
const int updates_per_second = 60;
volatile int game_time = 0;

int fps;
int frames_done;
int old_time;

int player_x;
int player_y=550;
int player_hurt_timer;
int player_health=100;
int player_fire_rate;
int player_fire_delay_rate;
int player_fire_rate_timer;
int player_laser_timer;
bool player_is_lasering;

int bullet_delay;
int helicopter_killcount;

float mouse_angle_radians;
float mouse_angle_allegro;

struct mine{
    int x;
    int y;
    int health=5;
    bool on_screen;
}mine[100];

struct raytracer{
    float x;
    float y;
    float vector_x;
    float vector_y;
    bool on_screen;

}raytracer[10];

struct helicopters{
    int x;
    int y=30;
    int direction=LEFT;
    int hurt_timer;
    float health=100;
    int fire_rate=20;
    int fire_timer;
    int movement_timer;
    float angle_radians;
    bool on_screen;
}helicopter[10];

struct bullet{
    float x;
    float y;
    float vector_x;
    float vector_y;
    bool on_screen=false;
    bool owner;
}bullets[100];

struct boxes{
    int x;
    int y;
    int type;
    bool on_screen=false;
}box[10];




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

// Random number generator. Use int random(highest,lowest);
int random(int newLowest, int newHighest)
{
  int lowest = newLowest, highest = newHighest;
  int range = (highest - lowest) + 1;
  int randomNumber = lowest+int(range*rand()/(RAND_MAX + 1.0));
  return randomNumber;
}
//Collision between 2 boxes
bool collision(float xMin1, float xMax1, float xMin2, float xMax2, float yMin1, float yMax1, float yMin2, float yMax2)
{
  if (xMin1 < xMax2 && yMin1 < yMax2 && xMin2 < xMax1 && yMin2 < yMax1){
    return true;
  }
  return false;
}

//A function to streamline error reporting in file loading
void abort_on_error(const char *message){
	 set_window_title("Error!");
	 if (screen != NULL){
	    set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
	 }
	 allegro_message("%s.\n %s\n", message, allegro_error);
	 exit(-1);
}
//Mine factory
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
//Helicopter factory
void create_helicopter(int newAmount){
    int helicopter_made=0;
    for(int i=0; i<10; i++){
        if(!helicopter[i].on_screen && helicopter_made<newAmount){
            helicopter_made++;
            helicopter[i].health=100;
            helicopter[i].on_screen=true;
            helicopter[i].x=random(1,600);
            helicopter[i].y=random(1,200);
            helicopter[i].direction=LEFT;
        }

    }

}

//Raytracer
void raytrace(){
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

    }

}
//Bullet factory
void create_bullet(int newX, int newY, bool newOwner, float newAngle, float newSpeed){
    bool bullet_made=false;
    for(int i=0; i<100; i++){
        if(!bullets[i].on_screen && !bullet_made){
            bullet_made=true;
            bullets[i].on_screen=true;
            bullets[i].x=newX;
            bullets[i].y=newY;
            bullets[i].vector_x=-newSpeed*cos(newAngle);
            bullets[i].vector_y=-newSpeed*sin(newAngle);
            bullets[i].owner=newOwner;
        }
    }
    bullet_delay=0;
}
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
    bullet_delay=0;
}


//Finds angle of point 2 relative to point 1
float find_angle(int x_1, int y_1, int x_2, int y_2){
    float tan_1;
    float tan_2;
    if(x_1-x_2!=0 && y_1-y_2!=0){
        tan_1=y_1-y_2;
        tan_2=x_1-x_2;
    }

    return atan2(tan_1,tan_2);
}


void update(){
    if((key[KEY_LEFT] || key[KEY_A]) && player_x>1)player_x-=5;
    if((key[KEY_RIGHT] || key[KEY_D]) && player_x<750)player_x+=5;

    if(player_health<1){
        close_button_pressed=true;
    }


    player_hurt_timer--;
    player_fire_rate_timer--;
    player_laser_timer--;

    if(player_fire_rate_timer<1){
        player_fire_rate=20;
        player_fire_delay_rate=9;
    }

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


    for(int i=0; i<10; i++){
        if(helicopter[i].on_screen){

            if(random(1,1000)==1)create_mine(helicopter[i].x,helicopter[i].y);

            helicopter[i].angle_radians=find_angle(helicopter[i].x+100,helicopter[i].y+30,player_x+15,player_y+20);

            if(helicopter[i].direction==LEFT)helicopter[i].x-=5;
            if(helicopter[i].direction==RIGHT)helicopter[i].x+=5;



            if(helicopter[i].x>600){
                helicopter[i].direction=HOVER;
                helicopter[i].movement_timer++;
                if(helicopter[i].movement_timer>120){
                    helicopter[i].direction=LEFT;
                    helicopter[i].movement_timer=0;
                }
            }
            if(helicopter[i].x<0){
                helicopter[i].direction=HOVER;
                helicopter[i].movement_timer++;
                if(helicopter[i].movement_timer>120){
                    helicopter[i].direction=RIGHT;
                    helicopter[i].movement_timer=0;
                }
            }
            if(helicopter[i].fire_rate<helicopter[i].fire_timer){
                create_bullet(helicopter[i].x+100,helicopter[i].y+30,HELICOPTER,helicopter[i].angle_radians,10);
                helicopter[i].fire_timer=0;
            }

        }

            helicopter[i].hurt_timer--;
            helicopter[i].fire_timer++;




            if(helicopter[i].health<1){


                if(helicopter_killcount<15){
                    if(random(1,15-helicopter_killcount)==1){
                        create_box(helicopter[i].x,helicopter[i].y,0);
                    }
                }else if(random(1,2)==1)create_box(helicopter[i].x-76,helicopter[i].y,0);

                if(helicopter_killcount<20){
                    if(random(1,20-helicopter_killcount)==1){
                        create_box(helicopter[i].x,helicopter[i].y,2);
                    }
                }else create_box(helicopter[i].x-76,helicopter[i].y,2);




                create_box(helicopter[i].x+76,helicopter[i].y,1);
                helicopter[i].on_screen=false;
                helicopter[i].health=100;
                if(helicopter_killcount<10)create_helicopter(1);
                if(helicopter_killcount>9)create_helicopter(random(1,2));

                helicopter_killcount++;


            }

    }

    mouse_angle_radians=find_angle(player_x+15,player_y+20,mouse_x,mouse_y);
    mouse_angle_allegro=mouse_angle_radians*40.5845104792;


    bullet_delay++;



    if((key[KEY_SPACE] || mouse_b & 1) && bullet_delay>player_fire_delay_rate ){
        if(player_laser_timer<1)create_bullet(player_x+15,player_y+20,PLAYER,mouse_angle_radians,player_fire_rate);
        else raytrace();
    }
    for(int i=0; i<100; i++){

        if(bullets[i].on_screen){
            bullets[i].x+=bullets[i].vector_x;
            bullets[i].y+=bullets[i].vector_y;
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
                    if(collision(mine[j].x,mine[j].x+200,bullets[i].x,bullets[i].x+5,mine[j].y,mine[j].y+40,bullets[i].y,bullets[i].y+5) && bullets[i].on_screen && bullets[i].owner){
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


           if(bullets[i].x>800 || bullets[i].y>600 || bullets[i].x<0 || bullets[i].y<0)bullets[i].on_screen=false;
        }
    }
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
             }


        }
    }






}

void draw(){
    draw_sprite(buffer,background,0,0);


    if(player_hurt_timer<1)draw_sprite(buffer,player,player_x,player_y);
    else draw_sprite(buffer,player_hurt,player_x,player_y);

    rectfill(buffer,550,10,754,30,makecol(0,0,0));
    rectfill(buffer,552,12,752,28,makecol(255,0,0));
    rectfill(buffer,552,12,552+(player_health*2),28,makecol(0,255,0));


    for(int i=0; i<100; i++){
        if(bullets[i].on_screen){

            if(bullets[i].owner){
                rectfill(buffer,bullets[i].x,bullets[i].y,bullets[i].x+5,bullets[i].y+5,makecol(0,0,0));
                rectfill(buffer,bullets[i].x+1,bullets[i].y+1,bullets[i].x+4,bullets[i].y+4,makecol(0,0,0));
                rectfill(buffer,bullets[i].x+2,bullets[i].y+2,bullets[i].x+3,bullets[i].y+3,makecol(0,0,0));
            }else{
                rectfill(buffer,bullets[i].x,bullets[i].y,bullets[i].x+5,bullets[i].y+5,makecol(255,0,0));
                rectfill(buffer,bullets[i].x+1,bullets[i].y+1,bullets[i].x+4,bullets[i].y+4,makecol(255,0,0));
                rectfill(buffer,bullets[i].x+2,bullets[i].y+2,bullets[i].x+3,bullets[i].y+3,makecol(255,0,0));
            }
        }
    }

    for(int i=0; i<10; i++){
        if(helicopter[i].on_screen){

            rectfill(buffer,helicopter[i].x,helicopter[i].y-10,helicopter[i].x+102,helicopter[i].y+2,makecol(0,0,0));
            rectfill(buffer,helicopter[i].x+2,helicopter[i].y-8,helicopter[i].x+100,helicopter[i].y,makecol(255,0,0));
            rectfill(buffer,helicopter[i].x+2,helicopter[i].y-8,helicopter[i].x+(helicopter[i].health),helicopter[i].y,makecol(0,255,0));


            if(helicopter[i].hurt_timer<1)draw_sprite(buffer,helicopter_image,helicopter[i].x,helicopter[i].y);
            if(helicopter[i].hurt_timer>0)draw_sprite(buffer,helicopter_hurt,helicopter[i].x,helicopter[i].y);

        }
    }
    for(int i=0; i<10; i++){
        if(box[i].on_screen){

            if(box[i].type==0)draw_sprite(buffer,box_machinegun,box[i].x,box[i].y);
            if(box[i].type==1)draw_sprite(buffer,box_health,box[i].x,box[i].y);
            if(box[i].type==2)draw_sprite(buffer,box_laserbeam,box[i].x,box[i].y);


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
    if(player_is_lasering)rotate_sprite(buffer,laserbeam,player_x-780,player_y,itofix(mouse_angle_allegro));
    player_is_lasering=false;
    textprintf_ex(buffer,font,20,20,makecol(0,0,0),-1,"Helicopter Killcount: %i",helicopter_killcount);

    draw_sprite(buffer,cursor,mouse_x-10,mouse_y-10);
    draw_sprite(screen,buffer,0,0);


}







void setup(){
    buffer=create_bitmap(800,600);



    create_helicopter(1);


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

    if (!(player = load_bitmap("player.png", NULL)))
      abort_on_error("Cannot find image player.png\nPlease check your files and try again");

    if (!(player_hurt = load_bitmap("player_hurt.png", NULL)))
      abort_on_error("Cannot find image player_hurt.png\nPlease check your files and try again");

    if (!(background = load_bitmap("background.png", NULL)))
      abort_on_error("Cannot find image background.png\nPlease check your files and try again");

    if (!(cursor = load_bitmap("cursor.png", NULL)))
      abort_on_error("Cannot find image cursor.png\nPlease check your files and try again");\

    if (!(bullet_image = load_bitmap("bullet_image.png", NULL)))
      abort_on_error("Cannot find image bullet_image.png\nPlease check your files and try again");

    if (!(helicopter_bullet_image = load_bitmap("helicopter_bullet_image.png", NULL)))
      abort_on_error("Cannot find image helicopter_bullet_image.png\nPlease check your files and try again");

    if (!(helicopter_image = load_bitmap("helicopter.png", NULL)))
      abort_on_error("Cannot find image helicopter.png\nPlease check your files and try again");

    if (!(helicopter_hurt = load_bitmap("helicopter_hurt.png", NULL)))
      abort_on_error("Cannot find image helicopter_hurt.png\nPlease check your files and try again");

    if (!(box_machinegun = load_bitmap("box_machinegun.png", NULL)))
      abort_on_error("Cannot find image box_machinegun.png\nPlease check your files and try again");

    if (!(box_health = load_bitmap("box_health.png", NULL)))
      abort_on_error("Cannot find image box_health.png\nPlease check your files and try again");

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
