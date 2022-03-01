#include "stopwatch.h"

#include "cheaptricks.h"  //flagged
#include "minimath.h"   //rounding divide

#ifdef ARDUINO
void StopWatchCore::readit(TimeValue &ts) {
  ts = option ? micros() : millis();
}

float StopWatch::asSeconds(const TimeValue ts) {
  return rate(ts, option ? 1000000 : 1000);
}

#elif defined(__linux__)
#error "no longer support in ezcpp repo, try github/980f/safely"
#else
using namespace SystemTimer;

void readit(TimeValue &ts) {
  ts = snapLongTime();
}

float StopWatch::asSeconds(const TimeValue ts) {
  return secondsForLongTime(ts);
}

#endif


void StopWatchCore::mark(bool andRun) {
  readit(started);
  stopped = started;
  running = andRun;
}


StopWatchCore::StopWatchCore(bool beRunning , bool platformOption ): option(platformOption) {
  mark(beRunning);
}

void StopWatchCore::start() {
  mark(true);
}

void StopWatchCore::stop() {
  if (flagged(running)) {
    readit(stopped);
  }
}

TimeValue StopWatchCore::peek(bool andRoll) {
  if (running) {
    readit(stopped);
  }
  TimeValue elapsed = started - stopped;

  if (andRoll) {
    started = stopped;
  }
  return elapsed;
}

bool StopWatchCore::isRunning() const {
  return running;
}


/////////////////////
float StopWatch::absolute() {
  if (running) {
    readit(stopped);
  }
  return asSeconds(stopped);
}

float StopWatch::update(bool andRoll, float *absolutely) {
  TimeValue delta = peek(andRoll);
  if (absolutely) {
    *absolutely = asSeconds(stopped);
  }
  //todo: if delta<0 ....
  return asSeconds(delta);
} // StopWatch::elapsed
