#include "item.h"

item::item(BITMAP* newSprite, int newX, int newY, int newType){

  sprite = newSprite;

  width = sprite -> w;
  height = sprite -> h;

  x = newX;
  y = newY;

  type = newType;

}
item::~item(){


}

void item::update(){

  if(y<550)
    y+=5;
}


void item::draw(BITMAP *tempBitmap){

  draw_sprite(tempBitmap,sprite,x,y);


}
