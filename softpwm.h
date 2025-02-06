#pragma once
#include "sharedtimer.h"

/**
 * this could have been implemented with two PolledTimer's triggering each other, but that uses more critical resources than coding it directly.
 */
class SoftPWM_base: public SharedTimer {
protected:
  Ticks pair[2];//indices: is low=0, high=1
  bool phase;

  /** we override the 'count complete' event and switch to the other interval time value */
  void onDone() override {
    phase = !phase;
    restart(pair[phase] - 1);//# the sharedtimer stuff adds a 1 for good luck, we don't need no stinking luck. //todo: guard against a zero input
    onToggle(phase);
  }

public:
  operator bool() const {
    return phase;
  }

  void trigger(bool from){
    phase=from;
    onDone();
  }

  void setPhase(bool highness,Ticks ticks){
    pair[highness] = ticks ?: 1;
  }

  void setCycle(Ticks low, Ticks high){
    pair[0] = low;
    pair[1] = high;
  }

  SoftPWM_base(Ticks low, Ticks high, bool andStart = false);

public:
  //since onDone is virtual we make this virtual also. We should try to replace that with a function member.
  virtual void onToggle(bool /*on*/){
    // a hook, overload with a pin toggle to make a PWM output.
  }

};

class SoftPWM:public SoftPWM_base {
  protected:
  using Actor= void(*)(bool phase);
  public:
  Actor actor;//gets called from Timer ISR, perhaps just set 
  virtual void onToggle(bool on){
    if(actor){
      (*actor)(on);
    }
  }
};

/** pwm action is to increment one counter on rise, the other on fall. */
class PolledSoftPWM:public SoftPWM_base {  
  public:
  PolledSoftPWM (Ticks low, Ticks high, bool andStart = false):SoftPWM_base(low,high,andStart){}
  unsigned changes[2];
  void onToggle(bool on) override{
    ++changes[on];
  }
  /** @returns whether we are in the low part of the low then high cycle. */
  operator bool(){
    return changes[0]!=changes[1];//using != instead of > to deal with wraparound.
  }
};