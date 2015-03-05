#pragma once

#include "eztypes.h"
#include "bundler.h"
#include "cheapTricks.h"

/**
  * an isr will determine that the given time has expired,
  * but the interested code will have to look at object to determine that the event occurred.
  *
  * Cancellable retriggerable monostable.
  */
class PolledTimer:public Bundler<PolledTimer> {
protected:
  bool done;
  u32 systicksRemaining;
public:
  PolledTimer(void);
  virtual void restart(u32 ticks); //add to list if not present, never gets removed so don't add dynamic objects.
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
