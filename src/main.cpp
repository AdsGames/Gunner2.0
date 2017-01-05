#include "tools.h"
#include "projectile.h"
#include "enemy.h"

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

int player_x;
int player_y=550;
int player_hurt_timer;
int player_health=100;
int player_fire_rate;
int player_fire_delay_rate;
int player_fire_rate_timer;
int player_laser_timer;
int player_bouncy_timer;
bool player_is_lasering;

int bullet_delay;
int helicopter_killcount;
int jump_timer=21;

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

struct boxes{
    int x;
    int y;
    int type;
    bool on_screen=false;
}box[10];

std::vector<projectile> game_projectiles;
std::vector<enemy> game_enemies;


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
   game_enemies.push_back(enemy(helicopter_image,helicopter_hurt));

}

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
//Bullet factory
projectile create_bullet(int newX, int newY, bool newOwner, float newAngle, float newSpeed){
  game_projectiles.push_back(projectile(newX,newY,newOwner,newAngle,newSpeed));
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





void update(){
    if((key[KEY_LEFT] || key[KEY_A]) && player_x>1)player_x-=10;
    if((key[KEY_RIGHT] || key[KEY_D]) && player_x<750)player_x+=10;

    if(player_health<1){
        close_button_pressed=true;
    }


    player_hurt_timer--;
    player_fire_rate_timer--;
    player_laser_timer--;
    player_bouncy_timer--;

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


    for(int i=0; i<game_enemies.size(); i++){
        game_enemies[i].update(player_x,player_y);


    }



    mouse_angle_radians=find_angle(player_x+15,player_y+20,mouse_x,mouse_y);
    mouse_angle_allegro=mouse_angle_radians*40.5845104792;


    bullet_delay++;
    jump_timer++;
    if((key[KEY_SPACE]||key[KEY_W]) && jump_timer>20){
      jump_timer=0;
    }

    if(jump_timer > 0 && 10 > jump_timer){
      player_y-=20;
    }
    if(jump_timer>10 && jump_timer<20){
      player_y+=20;
    }


    if((mouse_b & 1) && bullet_delay>player_fire_delay_rate ){
        if(player_laser_timer<1)create_bullet(player_x+15,player_y+20,PLAYER,mouse_angle_radians,player_fire_rate);
        else raytrace();
    }
    for(int i=0; i<game_projectiles.size(); i++){
      game_projectiles[i].update();
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
                if(box[i].type==3){
                    player_bouncy_timer=600;
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


    for(int i=0; i<game_projectiles.size(); i++){
      game_projectiles[i].draw(buffer);
    }

    for(int i=0; i<game_enemies.size(); i++){
       game_enemies[i].draw(buffer);
    }
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
