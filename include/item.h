#ifndef ITEM_H
#define ITEM_H


#include "tools.h"

class item{
  public:
    item(BITMAP* newSprite, int newX, int newY, int newType);
    ~item();
    void update();
    void draw(BITMAP *tempBitmap);

    int get_x(){return x;}
    int get_y(){return y;}

  private:
    int x;
    int y;
    int type;

    BITMAP* sprite;


};

#endif
