#include "polledtimer.h"

/**
  * an isr will determine that the given time has expired,
  * but the interested code will have to look at object to determine that the event occurred.
  * as of this note all timers are touched every cycle even if they are finished.
  * using 2 lists would make the isr faster, but all the restarts slower and restarts dominate first use.
  */

/** name required by systick.h. someday we'll get alias for static class member worked out */
void PolledTimerServer(void) {
  PolledTimer::forAll(&PolledTimer::check);
} /* onTick */


void PolledTimer::check(){
  if(!done){
    if(--systicksRemaining==0){
      onDone();
    }
  }
}


PolledTimer::PolledTimer(void){
  done = 1;
  systicksRemaining = 0;
}

/** typically this is overloaded if latency is important.*/
void PolledTimer::onDone(void){
  done = true;
}

void PolledTimer::restart(u32 value){
  systicksRemaining = value + 1; //to ensure minimum wait even if tick fires while we are in this code.
  if(value > 0) {//# leave expanded for debug
    done = 0; //this makes this retriggerable
  } else { //negative waits are instantly done.
    done = 1;
    //this is a failure to start so we don't call the onDone..
  }
} /* restart */

void PolledTimer::restart(float seconds){
  if(seconds<=0){
    return;
  }
  restart(u32(seconds));
}

void PolledTimer::freeze(){
  done=1;//precludes isr touching remaining time, and onDone doesn't get called.
}

////////////////////////////

bool CyclicTimer::hasFired(){
  //maydo: LOCK, present intended uses can miss a tick without harm so we don't bother.
  ClearOnExit <u32> z(fired); //trivial usage of ClearOnExit, for testing that.
  return fired != 0;
}

void CyclicTimer::onDone(){
  ++fired;
  //pointless: PolledTimer::onDone();
  retrigger();
}