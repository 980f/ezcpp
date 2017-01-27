#include "circularpointer.h"


CircularPointerVoidly::CircularPointerVoidly(void *base, unsigned step):
  pointer(reinterpret_cast<Bytely*>(base)),step(step),base(reinterpret_cast<Bytely*>(base))
{
  //end is TRASH. Its logic was painful to try to do in a constructor initialization expression.
}

void CircularPointerVoidly::rewind(){
  pointer = base;
}

void CircularPointerVoidly::move(int amount){
  if(pointer){
    Bytely * proposed = pointer + amount*step;
    if(proposed >= end) {
      pointer -= end-base;
    } else if(proposed < base) {
      pointer += end-base;
    } else {
      pointer = proposed;
    }
  }
}

void CircularPointerVoidly::increment(){
  if(pointer){
    pointer+=step;
    if(pointer>=end){
      pointer=base;
    }
  }
}

void CircularPointerVoidly::decrement(){
  if(pointer){
    if(pointer==base){
      pointer=end;
    } else {
      pointer-=step;
    }
  }
}
