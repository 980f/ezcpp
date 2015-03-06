#include "boolish.h"

bool CachedBoolish::operator =(bool on){
  bit=on;
}

CachedBoolish::operator bool(){
  return bit;
}
