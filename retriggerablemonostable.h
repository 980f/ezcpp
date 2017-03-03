#ifndef RETRIGGERABLEMONOSTABLE_H
#define RETRIGGERABLEMONOSTABLE_H

#include "polledtimer.h"
#include "boolish.h"
class RetriggerableMonostable : public PolledTimer, public Boolish {
  const BoolishRef &Q;//so many of our Boolishnesses are const'ed we make this class work with them, will have to proxy around constness to use with assignable Bools.
  Ticks period;
public:
  RetriggerableMonostable(const BoolishRef &output, Ticks ticks = 0);
  /** turn the bit on, start timer*/
  void trigger();

  /** hard reset, changes Q */
  void stop();

  /** turn the bit off */
  void onDone() override;

  virtual bool operator =(bool triggerOrStop) override {
    if(triggerOrStop) {
      trigger();
    } else {
      stop();
    }
    return triggerOrStop;
  }

  /** @returns whether this guy turned the Q on, but hasn't turned it off.
   * This is not the same thing as the Q actually being on.*/
  virtual operator bool() const override {
    return !PolledTimer::isDone();
  }

}; // class RetriggerableMonostable

#endif // RETRIGGERABLEMONOSTABLE_H
