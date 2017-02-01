#include "boolish.h"

bool CachedBoolish::operator =(bool on){
  bit=on;
  return on;//in case this is a pin and the pin is external not tracking what we write. IE this return is defined to be a pass through.
}

CachedBoolish::operator bool(){
  return bit;
}
