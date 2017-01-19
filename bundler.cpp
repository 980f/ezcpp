#include "bundler.h"

//unit test


/**
  * an isr will determine that the given time has expired,
  * but the interested code will have to look at object to determine that the event occurred.
  */
class Gropued:public Bundler<Gropued> {
public:
  bool done=true;
  unsigned systicksRemaining=71;
public: //accessors for the system timer
  Gropued(){
    systicksRemaining=179;
  }
  static void onTick(void){
    forAll(&Gropued::onDone);
  } //to be called periodically, expects to be called in an isr.
  //extend and overload to have something done within the timer interrupt service routine:
  void onDone(void);
};

//Gropued A;
//Gropued B;
