#ifndef ITEM_H
#define ITEM_H


#include "tools.h"

class item{
  public:
    item(BITMAP* newSprite, int newX, int newY, int newType);
    ~item();
    void update();
    void draw(BITMAP *tempBitmap);
  private:
    int x;
    int y;
    int type;

    BITMAP* sprite;


};

#endif
