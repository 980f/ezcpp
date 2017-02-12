#include "stopwatch.h"

#include "cheaptricks.h"  //flagged

#ifdef ARDUINO

extern TimeValue millis();
void readit(TimeValue &ts){
  ts=millis();
}

double StopWatch::asSeconds(const TimeValue ts){
  return (ts)*1000;//arduino hardcoded to millisecond.
}

#elif defined(__linux__)
void readit(TimeValue &){
//  ts=snapLongTime();
}

double StopWatch::asSeconds(const TimeValue ){
//  return secondsForLongTime(ts);
}

#else
#include "systick.h"
using namespace SystemTimer;

void readit(TimeValue &ts){
  ts=snapLongTime();
}

double StopWatch::asSeconds(const TimeValue ts){
  return secondsForLongTime(ts);
}

#endif

StopWatchCore::StopWatchCore(bool beRunning,bool /*realElseProcess*/){
  readit(started);
  stopped = started;
  running = beRunning;
}

void StopWatchCore::start(){
  readit(started);
  running = true;
}

void StopWatchCore::stop(){
  if(flagged(running)) {
    readit(stopped);
  }
}

TimeValue StopWatchCore::peek(bool andRoll){
  if(running){
    readit(stopped);
  }
  TimeValue elapsed=started-stopped;

  if(andRoll){
    started=stopped;
  }
  return elapsed;
}

bool StopWatchCore::isRunning() const {
  return running;
}

/////////////////////
double StopWatch::absolute(){
  if(running) {
    readit(stopped);
  }
  return asSeconds(stopped);
}

double StopWatch::elapsed(double *absolutely){
  TimeValue delta=peek(false);
  if(absolutely) {
    *absolutely = asSeconds(stopped);
  }
  //todo: if delta<0 ....
  return asSeconds(delta);
} // StopWatch::elapsed


double StopWatch::roll(double *absolutely){
  TimeValue delta=peek(true);
  if(absolutely) {
    *absolutely = asSeconds(stopped);
  }
  return asSeconds(delta);
}
