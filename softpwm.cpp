#include "softpwm.h"

SoftPWM_base::SoftPWM_base(Ticks low, Ticks high, bool andStart){
  setCycle(low,high);
  if(andStart) {
    onDone();
  }
}
