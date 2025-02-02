#pragma once

/** quadrature table, forward motion
 *       ____      ___
 * A ___/    \____/
 *         ____      ___
 * B _____/    \____/
 *
    if B is low when A goes high we increment.
    if B is high when A goes high we decrement.
    if B is high when A goes low we increment.
    if B is low when A goes low we decrement.
    if A is high when B goes high we increment.
    if A is low when B goes high we decrement.
    if A is low when B goes low we increment.
    if A is high when B goes low we decrement.

2 bit Grey-code: 0,1,3,2 as lsbs is lsb-straight = lsb-grey^msb-grey, msb-straight = msb-grey
  If we are using A as the clock channel while sampling B for direction then A is the 2 bit lsb, B is the 2 bit msb.

  If we are counting on either edge of A we advance the counter 2 for every edge, if we count just one edge of A (rising please) then we jump 4 for each count.
  Regardless of which edges we counted we use the 2 signals, grey code corrected, as the 2 lsbs of the position.

 */

/** the counting logic of quadrature decoding. To be called by some other module which tends to the physical inputs.  */
class QuadratureCounter {
public: // eventually protect
  int location = 0;
  int dirstep = 0;//birection and magnitude of 1,2,4
  bool homed; //whether position is actually known

public:
  /** one arbitrarily selects one input as primary, which designation defines which way is forward. 
   * this step method is to be called for every change of the related edge, passed the level of the other input
   */
  void stepOne(bool samePhase) {
    if (samePhase) {
      --location;
    } else {
      ++location;
    }
  }

  /** converse of stepOne with respect to inputs*/
  void stepOther(bool samePhase) {
    if (samePhase) {
      ++location;
    } else {
      --location;
    }
  }

    /** when cruising we set stepsize to 2 */
  enum Stepby { //the values are only coincidentally 1<<0, 1<<1, and 1<<2, these are not bit mask definitions.
    BothPhases= 1, //gets called
    BothEdges = 2,
    OneEdge = 4
  };

  /** when cruising we only look at one phase, and possibly one edge.
   * the dirstep records the direction and if one edge then set to 4, if both edges set to 2. 
   * One should only cruise when one is sure that the direction cannot change, presumably due to inertia.
   */
  void cruiseDirection(int dirstep) { this->dirstep = dirstep; }

  /** call this with each primary edge when cruising */
  void cruiseStep() { location += dirstep; }

  /** when using a hardware counter we retain the cruiseDirection setting and report the merge of the last incremental update with the count value.
   * @returns the new location value after applying @param count steps */
  int fly(unsigned count) const { return location + dirstep * count; }

  /** when a 16 bit hardware counter wraps we need to update our internal value (unless we keep a rollover count with the counter). 
   * Also if we reset the hardware counter such as with an index mark, we need to accumulate the pre-cleared value. Unless the index is an absolute clear. */
  void rollover(unsigned count) { location += dirstep * count; }

  /** set only applies to location, not to any modality of the counting. */
  void set(int provided=0) { location = provided; homed=true; }
};
