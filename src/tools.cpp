#include "tools.h"


//Finds angle of point 2 relative to point 1
float find_angle(int x_1, int y_1, int x_2, int y_2){
    float tan_1=0.0;
    float tan_2=0.0;
    if(x_1-x_2!=0 && y_1-y_2!=0){
        tan_1=y_1-y_2;
        tan_2=x_1-x_2;
    }

    return atan2(tan_1,tan_2);
}
//Collision
bool collision(int xMin1, int xMax1, int xMin2, int xMax2, int yMin1, int yMax1, int yMin2, int yMax2){
  if (xMin1 < xMax2 && yMin1 < yMax2 && xMin2 < xMax1 && yMin2 < yMax1){
    return true;
  }
  return false;
}
// it randomly borked
//Checks if file exists
//bool fexists(const char *filename){
  //std::ifstream ifile(filename);
//  return ifile;
//}
float degrees_to_radians(float newDegrees){

    return newDegrees * PI / 180;
}

int random(int newLowest, int newHighest)
{
  int lowest = newLowest, highest = newHighest;
  int range = (highest - lowest) + 1;
  int randomNumber = rand() % range + lowest;
  return randomNumber;
}
// Random float
float randomf(float newLowest, float newHighest){
  float lowest = newLowest, highest = newHighest;
  float range = (highest - lowest) + 1;
  float randomNumber = lowest + float( range * rand()/(RAND_MAX + 1.0));
  return randomNumber;
}

// Convert int to string
std::string convertIntToString(int number){
  std::stringstream ss;
  ss << number;
  return ss.str();
}

//Convert double to string
std::string convertDoubleToString(double number){
  std::stringstream ss;
  ss << number;
  return ss.str();
}

//Convert bool to string
std::string convertBoolToString(bool boolean){
  std::stringstream ss;
  ss << boolean;
  return ss.str();
}

//Finds angle of point 2 relative to point 1
float find_angle( float x_1, float y_1, float x_2, float y_2){
  return atan2(y_1 - y_2, x_1 - x_2);
}

//Finds distance between 2 points
float find_distance( float x_1, float y_1, float x_2, float y_2){
  return hypot(x_1 - x_2, y_1 - y_2);
}

// Fade in
void highcolor_fade_in(BITMAP* bmp_orig, int speed){
  BITMAP* bmp_buff = create_bitmap(SCREEN_W,SCREEN_H);
  BITMAP* str_orig = create_bitmap( SCREEN_W, SCREEN_H);
  stretch_sprite( str_orig, bmp_orig, 0, 0, SCREEN_W, SCREEN_H);

  if ( speed<=0)
    speed=16;

  for(int a=0; a<256; a+=speed){
    clear( bmp_buff);
    set_trans_blender( 0, 0, 0, a);
    draw_trans_sprite( bmp_buff, str_orig, 0, 0);
    vsync();
    stretch_sprite( screen, bmp_buff, 0, 0,  SCREEN_W, SCREEN_H);
  }
  stretch_sprite( screen, str_orig, 0, 0,  SCREEN_W, SCREEN_H);
}

// Fade out
void highcolor_fade_out(int speed){
  BITMAP* bmp_buff = create_bitmap(SCREEN_W,SCREEN_H);
  BITMAP *bmp_orig = create_bitmap( SCREEN_W, SCREEN_H);
  blit(screen,bmp_orig,0,0,0,0,SCREEN_W,SCREEN_H);

  if ( speed<=0)
    speed=16;

  for( int a=255 - speed; a>0; a-=speed){
    clear( bmp_buff);
    set_trans_blender( 0, 0, 0, a);
    draw_trans_sprite( bmp_buff, bmp_orig, 0, 0);
    vsync();
    stretch_sprite( screen, bmp_buff, 0, 0,  SCREEN_W, SCREEN_H);
  }
  destroy_bitmap( bmp_orig);
  rectfill( screen, 0, 0,  SCREEN_W, SCREEN_H, makecol(0,0,0));
}

/*
 *  ERROR REPORTING
 */
void abort_on_error(const char *message){
	 if (screen != NULL){
	    set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
	 }
	 allegro_message("%s.\n %s\n", message, allegro_error);
	 exit(-1);
}

