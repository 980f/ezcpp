#include "sharedtimer.h"

#include <cheaptricks.h>//take()

#include "tableofpointers.h" //for linker generated table of timers sharing a timebase.
/**
  * an isr will determine that the given time has expired,
  * but the interested code will have to look at object to determine that the event occurred.
  * as of this note all timers are touched every cycle even if they are finished.
  * using 2 lists would make the isr faster, but all the restarts slower and restarts dominate first use.
  *
  * As of this implementation there can be only one instance of a shared timer, historically tied to the cortexM ssystick.
  * To share other timers we need other instances of SharedTimerServer and add a timerid to the timer table name. The latter is costly in that we are using macros to make this facility easy to use and macros do not support default arguments.
  *
  */

MakeTable(SharedTimer);

/** name required by systick.h. someday we'll get alias for static class member worked out */
void SharedTimerServer() {
  ForTable(SharedTimer){
    (*it)->check();
  }
} /* onTick */


void SharedTimer::check(){
  if(running){
    if(--systicksRemaining==0){
      onDone();//default method clears running, i.e. simple SharedTimers are one-shots.
    }
  }
}

SharedTimer::SharedTimer(){
  running = 0;
  systicksRemaining = 0;
}

SharedTimer::~SharedTimer(){
  //#trivial
  running=0;//reduce use-after-free consequences
}

/** typically this is overloaded if latency is important.*/
void SharedTimer::onDone(){
  running = 0;
}

void SharedTimer::restart(Ticks value){
  systicksRemaining = value + 1; //to ensure minimum wait even if tick fires while we are in this code.
  if(value > 0) {//# leave expanded for debug
    running = 1; //this makes this retriggerable
  } else { //negative waits are instantly done.
    running = 0;
    //this is a failure to start so we don't call the onDone..
  }
} /* restart */

void SharedTimer::freeze(){
  running = false;//precludes isr touching remaining time, and onDone doesn't get called.
}

////////////////////////////

bool CyclicTimer::hasFired(){
  return take(fired) != 0;
}

void CyclicTimer::onDone(){
  ++fired;
  //pointless: SharedTimer::onDone();
  retrigger();
}
