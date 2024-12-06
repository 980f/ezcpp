#ifndef STOPWATCH_H
#define STOPWATCH_H

#include "eztypes.h"

#ifdef ARDUINO
#include <Arduino.h> // needed for non-arduino IDE IDE's.
using TimeValue = decltype(millis());
#elif defined(__linux__)
#error "use github/980f/safely repo for better time services"
#else
#include "systick.h"  //980F wrapper for cortexM systick timer
using TimeValue = decltype(SystemTimer::snapLongTime());
#endif


/** an interval timer

  StopWatchCore leaves out floating point
  @see StopWatch for more convenient object, at cost of the f.p. library being linked.

*/
class StopWatchCore {

protected:
    void readit(TimeValue &ts) ;
  TimeValue started;
  TimeValue stopped;
  bool running;
    bool option;
public:
  /** @param beRunning is whether to start timer upon construction.
    @param platformOption is for any per object per platform tweak.
    defaults are set for timing function execution */
    StopWatchCore(bool beRunning = true, bool platformOption = false);
    /** record 'now' in both start and stop, and conditionally run. */
    void mark(bool andRun);
  /** use start and stop for non-periodic purposes*/
  void start();
  void stop();
    /** erase any value, to keep one from accidentally lingering after a stop */
    void clear() {
      mark(false);
    }
    /** read elapsed time as ticks, for when you can't afford the fp math in elapsed(), @param andRoll restarts timer when true */
  TimeValue peek(bool andRoll=false);
  /** convenient for passing around 'timeout pending' state */
  bool isRunning() const;
};

/**
  parts of the StopWatch concept that need floating point math
*/

class StopWatch:public StopWatchCore {
public:
  using StopWatchCore::StopWatchCore;
  /** timestamp reported as seconds since roughly the start of the application */
  double asSeconds(const TimeValue);
  /** common code for @see roll and elapsed. See those methods for details */
  float update(bool andRoll, float  *absolutely = 0);
  /** @returns elasped time and restarts interval. Use this for cyclic sampling. @param absolutely if not null gets the absolute time reading used in the returned value.*/
  double roll(double *absolutely=0);
  /** updates 'stop' if running and @returns time between start and stop as seconds. @param absolutely if not null gets the absolute time reading used in the returned value.*/
  double elapsed(double *absolutely=0);
  /** @return seconds of absolute time of stop, == now if running*/
  double absolute();

  /** @returns the number of cycles of frequency @param atHz that have @see elapsed() */
  unsigned cycles(double atHz, bool andRoll=true);
};


#endif // STOPWATCH_H
