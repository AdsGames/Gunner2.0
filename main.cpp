#include<allegro.h>
#include<alpng.h>
#include<time.h>
#include<cmath>

BITMAP* buffer;
BITMAP* gunman;
BITMAP* background;

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

struct bullet{
    float x;
    float y;
    float vector_x;
    float vector_y;
    bool on_screen=false;
}bullets[100];




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


//A function to streamline error reporting in file loading
void abort_on_error(const char *message){
	 set_window_title("Error!");
	 if (screen != NULL){
	    set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
	 }
	 allegro_message("%s.\n %s\n", message, allegro_error);
	 exit(-1);
}

void update(){
    if(key[KEY_LEFT] || key[KEY_A])player_x-=5;
    if(key[KEY_RIGHT] || key[KEY_D])player_x+=5;






}

void draw(){
    draw_sprite(buffer,background,0,0);
    draw_sprite(buffer,gunman,player_x,player_y);
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

    if (!(gunman = load_bitmap("gunman.png", NULL)))
      abort_on_error("Cannot find image gunman.png\nPlease check your files and try again");

    if (!(background = load_bitmap("background.png", NULL)))
      abort_on_error("Cannot find image background.png\nPlease check your files and try again");
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
