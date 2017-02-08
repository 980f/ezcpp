#include "cheaptricks.h"
#include "minimath.h"

bool isPresent(const char *flags, const char flag){
  int badStringLimiter = 26; //in case string pointer is garbage we don't want to read all of ram

  if(flags) {
    char probe;
    while((probe = *flags++) && badStringLimiter-- > 0) {
      if(flag == probe) {
        return true;
      }
    }
  }
  return false;
} /* isPresent */


bool changed(double&target, double newvalue){
  if(!nearly(target,newvalue)){
    target = newvalue;
    return true;
  } else {
    return false;
  }
}
