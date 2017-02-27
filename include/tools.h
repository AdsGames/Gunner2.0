#ifndef TOOLS_H
#define TOOLS_H


#define PLAYER TRUE
#define HELICOPTER FALSE

#define HEALTH 0

#define LEFT 0
#define RIGHT 1
#define HOVER 2



#include <allegro.h>
#include <alpng.h>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include <vector>



//Collision
extern bool collision(int xMin1, int xMax1, int xMin2, int xMax2, int yMin1, int yMax1, int yMin2, int yMax2);

//Finds angle of point 2 relative to point 1
extern float find_angle( float x_1, float y_1, float x_2, float y_2);
extern float find_distance( float x_1, float y_1, float x_2, float y_2);

// Mouse position including resolution difference
extern int mouseX();
extern int mouseY();

//Checks if file exists
extern bool fexists(const char *filename);

//Random number generator
extern int random(int newLowest, int newHighest);
extern float randomf(float newLowest, float newHighest);

//Convert int to string
extern std::string convertIntToString(int number);

//Convert double to string
extern std::string convertDoubleToString(double number);

//Convert bool to string
extern std::string convertBoolToString(bool boolean);

// Fade in and out
extern void highcolor_fade_in(BITMAP* bmp_orig, int speed);
extern void highcolor_fade_out(int speed);

// Next state
extern void set_next_state( int newState );

// Error reporting
extern void abort_on_error(const char *message);

extern float find_angle(int x_1, int y_1, int x_2, int y_2);

#endif // TOOLS_H
