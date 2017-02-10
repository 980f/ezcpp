#include "retriggerablemonostable.h"

RetriggerableMonostable::RetriggerableMonostable(const BoolishRef &output, Ticks ticks) :
  Q(output),period(ticks){

}

void RetriggerableMonostable::trigger(){
  Q = 1;
  restart(period);
}

void RetriggerableMonostable::stop(){
  Q = 0;
  freeze();
  systicksRemaining = 0;//preclude restarting.
}

void RetriggerableMonostable::onDone(){
  Q = 0;
}
