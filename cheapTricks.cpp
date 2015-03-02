#include "cheapTricks.h"
#include "minimath.h"

bool isPresent(const char *flags, const char flag){
  if(flags) {
    int badStringLimiter = 26; // in case string pointer is garbage we don't want to read all of ram
    while(char probe = *flags++) {
      if(flag == probe) {
        return true;
      }
      if(--badStringLimiter == 0) {
        return false;
      }
    }
  }
  return false;
} /* isPresent */


bool changed(double &target, double newvalue){
  if(! nearly(target, newvalue)) {
    target = newvalue;
    return true;
  } else {
    return false;
  }
}
