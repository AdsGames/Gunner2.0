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
BITMAP* helicopter;
BITMAP* helicopter_hurt;
BITMAP* box_machinegun;
BITMAP* box_health;

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

int helicopter_x;
int helicopter_y=30;
//Forward is true
int helicopter_direction=LEFT;
int helicopter_hurt_timer;
int helicopter_health=100;
int helicopter_fire_rate=20;
int helicopter_fire_timer;
int helicopter_movement_timer;

int bullet_delay;

float mouse_angle_radians;
float helicopter_angle_radians;

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
        player_x=-0;
        player_health=100;
    }

    helicopter_hurt_timer--;
    player_hurt_timer--;

    if(helicopter_direction==LEFT)helicopter_x-=5;
    if(helicopter_direction==RIGHT)helicopter_x+=5;



    if(helicopter_x>600){
        helicopter_direction=HOVER;
        helicopter_movement_timer++;
        if(helicopter_movement_timer>120){
            helicopter_direction=LEFT;
            helicopter_movement_timer=0;
        }
    }
    if(helicopter_x<0){
        helicopter_direction=HOVER;
        helicopter_movement_timer++;
        if(helicopter_movement_timer>120){
            helicopter_direction=RIGHT;
            helicopter_movement_timer=0;
        }
    }


    if(helicopter_health<1){
        create_box(helicopter_x,helicopter_y,random(0,1));
        helicopter_x=100;
        helicopter_direction=LEFT;
        helicopter_health=100;
    }
    mouse_angle_radians=find_angle(player_x+15,player_y+20,mouse_x,mouse_y);
    helicopter_angle_radians=find_angle(helicopter_x+100,helicopter_y+30,player_x+15,player_y+20);

    bullet_delay++;

    helicopter_fire_timer++;
    if(helicopter_fire_rate<helicopter_fire_timer){
        create_bullet(helicopter_x+100,helicopter_y+30,HELICOPTER,helicopter_angle_radians,10);
        helicopter_fire_timer=0;
    }
    if((key[KEY_SPACE] || mouse_b & 1) && bullet_delay>9 ){
        create_bullet(player_x+15,player_y+20,PLAYER,mouse_angle_radians,20);
    }
    for(int i=0; i<100; i++){

        if(bullets[i].on_screen){
           bullets[i].x+=bullets[i].vector_x;
           bullets[i].y+=bullets[i].vector_y;
           if(collision(helicopter_x,helicopter_x+200,bullets[i].x,bullets[i].x+5,helicopter_y,helicopter_y+40,bullets[i].y,bullets[i].y+5) && bullets[i].on_screen && bullets[i].owner){
                helicopter_health-=5;
                bullets[i].on_screen=false;
                helicopter_hurt_timer=3;
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
             }


        }
    }




}

void draw(){
    draw_sprite(buffer,background,0,0);
    if(helicopter_hurt_timer<1)draw_sprite(buffer,helicopter,helicopter_x,helicopter_y);
    else draw_sprite(buffer,helicopter_hurt,helicopter_x,helicopter_y);

    if(player_hurt_timer<1)draw_sprite(buffer,player,player_x,player_y);
    else draw_sprite(buffer,player_hurt,player_x,player_y);

    rectfill(buffer,10,10,214,30,makecol(0,0,0));
    rectfill(buffer,12,12,212,28,makecol(255,0,0));
    rectfill(buffer,12,12,12+(helicopter_health*2),28,makecol(0,255,0));

    rectfill(buffer,550,10,754,30,makecol(0,0,0));
    rectfill(buffer,552,12,752,28,makecol(255,0,0));
    rectfill(buffer,552,12,552+(player_health*2),28,makecol(0,255,0));


    for(int i=0; i<100; i++){
        if(bullets[i].on_screen){

            draw_sprite(buffer,bullet_image,bullets[i].x,bullets[i].y);
        }
    }
    for(int i=0; i<10; i++){
        if(box[i].on_screen){

            if(box[i].type==0)draw_sprite(buffer,box_machinegun,box[i].x,box[i].y);
            if(box[i].type==1)draw_sprite(buffer,box_health,box[i].x,box[i].y);

        }
    }

    draw_sprite(buffer,cursor,mouse_x-10,mouse_y-10);
    draw_sprite(screen,buffer,0,0);


}







void setup(){
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

    if (!(helicopter = load_bitmap("helicopter.png", NULL)))
      abort_on_error("Cannot find image helicopter.png\nPlease check your files and try again");

    if (!(helicopter_hurt = load_bitmap("helicopter_hurt.png", NULL)))
      abort_on_error("Cannot find image helicopter_hurt.png\nPlease check your files and try again");

    if (!(box_machinegun = load_bitmap("box_machinegun.png", NULL)))
      abort_on_error("Cannot find image box_machinegun.png\nPlease check your files and try again");

    if (!(box_health = load_bitmap("box_health.png", NULL)))
      abort_on_error("Cannot find image box_health.png\nPlease check your files and try again");
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
