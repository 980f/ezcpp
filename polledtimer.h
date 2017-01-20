#pragma once

#include "eztypes.h"
#include "cheaptricks.h"

/**
  * an isr will determine that the given time has expired,
  * but the interested code will have to look at object to determine that the event occurred.
  *
  * Cancellable retriggerable monostable.
  */
class PolledTimer {
protected:
  bool done;
  u32 systicksRemaining;
public:
  PolledTimer(void);
  virtual void restart(u32 ticks);
  void restart(float seconds);//float (not double) as is often in time critical code.
  /** stops countdown without triggering onDone() */
  void freeze();
  //what the timer isr calls:
  void check(void);
  /** called when systicksRemaining goes to 0.
   * Overload to have something done within the timer interrupt service routine */
  virtual void onDone(void);
  /** @returns whether this is no longer counting */
  inline bool isDone() const{
    return done;
  }
};


//the table is of pointers, the pointers are const, the object pointed to is not.
#define POLLEDTIMERTAG(prior) const __attribute((used, section(".table.PolledTimer." #prior )))

//puts a timer in the list
#define RegisterTimerAt(varble,priority) PolledTimer * POLLEDTIMERTAG(priority) PolledTimer##varble (&varble)
#define RegisterTimer(varble) RegisterTimerAt(varble,5)


/** automatic restart. If you are slow at polling it it may become done again while you are handling a previous done.
 * periodic event, with fairly low jitter. The period is fairly stable but the action routine can be delayed by other actions.
 */
class CyclicTimer : public PolledTimer {
protected:
  u32 period;
  u32 fired;
public:
  bool hasFired(void);

  operator bool (void){
    return hasFired();
  }

  void retrigger(void){
    //leave fired as is.
    PolledTimer::restart(period);
  }

  void restart(u32 ticks){
    PolledTimer::restart(period = ticks);
  }

  /** called when systicksRemaining goes to 0.
   * Overload to have something done within the timer interrupt service routine */
  virtual void onDone(void);

};
