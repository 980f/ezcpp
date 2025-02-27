#pragma once
#include <cstdint>

/** marker class for timing services */
using Ticks= uint32_t;
/** shared timer services. 
  * an isr will determine that the given time has expired, setting the done bit.
  * but the interested code will have to look at each object to determine that the event occurred OR
  * will have to derive a class and overload onDone().
  */


class SharedTimer {
protected:
  bool running; //changed from 'done' to 'running' so that we can init via joint ram clear rather than init code.
  Ticks systicksRemaining;

public:
  SharedTimer();
  
  //#HEAP virtual ~SharedTimer();//in case derived classes need significant destruction.

  virtual void restart(Ticks ticks);
  void restart(float seconds);//float (not double) as is often in time critical code.
  /** stops countdown without triggering onDone() */
  void freeze();
  //what the timer isr calls:
  void check();
  /** called when systicksRemaining goes to 0.
   * Overload to have something done <B>within</B> the timer interrupt service routine */
  virtual void onDone();
  /** @returns whether this is no longer counting */
  bool isDone() const{
    return !running;
  }
};


/** automatic restart. If you are slow at polling it it may become done again while you are handling a previous done.
 * periodic event, with fairly low jitter. The period is fairly stable but the action routine can be delayed by other actions.
 */
class CyclicTimer : public SharedTimer {
protected:
  Ticks period;
  unsigned fired;
public:
  explicit CyclicTimer(Ticks period, bool andStart = false) : period(period), fired(0) {
    if(andStart){
      retrigger();
    }
  }

  bool hasFired();

  operator bool() { // NOLINT
    return hasFired();
  }

  void retrigger(){
    //leave fired as is.
    SharedTimer::restart(period);
  }

  void restart(Ticks ticks) override {
    SharedTimer::restart(period = ticks);
  }

  /** called when systicksRemaining goes to 0.
   * Overload to have something done within the timer interrupt service routine */
  void onDone() override;
};


/** a server that will update all registered timers.
 * You must arrange for it to get called with each tick */
extern void SharedTimerServer();
//registration is compile time, your object must be static to be party to this service.
#define RegisterTimer(name) Register(SharedTimer,name)

//this can be preceded by 'static' if the object so created must be that.
#define MakeTimer(itsClass,name, ...) itsClass name( __VA_ARGS__); RegisterTimer(name)

