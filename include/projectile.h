#ifndef PROJECTILE_H
#define PROJECTILE_H


#include "tools.h"

class projectile{
  public:
    projectile(float newX, float newY, bool newOwner, float newAngle, float newSpeed);
    ~projectile();
    void draw(BITMAP* tempBitmap);
    void update();
    float get_x();
    float get_y();
    bool get_owner(){ return owner; }

  private:
    float x;
    float y;
    float x_velocity;
    float y_velocity;
    bool owner;


};
#endif
