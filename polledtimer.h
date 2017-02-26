#pragma once

#include "eztypes.h"
#include "cheaptricks.h"

/** marker class for timing services */
typedef u32 Ticks;
/** shared timer services. 
  * an isr will determine that the given time has expired, setting the done bit.
  * but the interested code will have to look at each object to determine that the event occurred OR
  * will have to derive a class and overload onDone().
  */


class PolledTimer {
protected:
  bool done;
  Ticks systicksRemaining;
public:
  PolledTimer(void);
  virtual void restart(Ticks ticks);
  void restart(float seconds);//float (not double) as is often in time critical code.
  /** stops countdown without triggering onDone() */
  void freeze();
  //what the timer isr calls:
  void check(void);
  /** called when systicksRemaining goes to 0.
   * Overload to have something done <B>within</B> the timer interrupt service routine */
  virtual void onDone(void);
  /** @returns whether this is no longer counting */
  inline bool isDone() const{
    return done;
  }
};


/** automatic restart. If you are slow at polling it it may become done again while you are handling a previous done.
 * periodic event, with fairly low jitter. The period is fairly stable but the action routine can be delayed by other actions.
 */
class CyclicTimer : public PolledTimer {
protected:
  Ticks period;
  u32 fired;
public:
  CyclicTimer(Ticks period,bool andStart=false):period(period){
    if(andStart){
      retrigger();
    }
  }

  bool hasFired(void);

  operator bool (void){
    return hasFired();
  }

  void retrigger(void){
    //leave fired as is.
    PolledTimer::restart(period);
  }

  void restart(Ticks ticks){
    PolledTimer::restart(period = ticks);
  }

  /** called when systicksRemaining goes to 0.
   * Overload to have something done within the timer interrupt service routine */
  virtual void onDone(void);

};


/** a server that will update all registered timers.
 * You must arrange for it to get called with each tick */
extern void PolledTimerServer(void);
//registration is compile time, your object must be static to be party to this service.
#define RegisterTimer(name) Register(PolledTimer,name)

