#ifndef PROJECTILE_H
#define PROJECTILE_H


#include "tools.h"

class projectile{
  public:
    projectile(float newX, float newY, bool newOwner, float newAngle, float newSpeed,bool newRicochet, int newWidth, int newHeight);
    ~projectile();
    void draw(BITMAP* tempBitmap);
    void update();
    float get_x();
    float get_y();
    int get_width();
    int get_height();
    int get_lifetime();
    bool get_owner(){ return owner; }

  private:
    float x;
    float y;
    float x_velocity;
    float y_velocity;
    int width;
    int height;
    bool owner;
    bool ricochet;
    int lifetime;


};
#endif
