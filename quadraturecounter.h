#ifndef QUADRATURECOUNTER_H
#define QUADRATURECOUNTER_H

/** quadrature table:
 *      ___     ___
 *  ___/   \___/
 *        ___     ___
 *  _____/   \___/
 *
 *  A:HL=++
 *  A:LH=++
 *  B:HH=++
 *  B:LL=++
 */

/** the counting logic of quadrature decoding. To be called by some other modules which tends to the physical inputs.  */
class QuadratureCounter {
public://eventually protect
  int location = 0;
  int dirstep = 0;
public:
  /** one arbitrarily selects one input as primary, which designation defines which way is forward.
   * this step method is to be called for every change of the related edge, passed 'a==b' comparing that signal's new state to the other signal's (steady) state.
   */
  template<bool primary> void step(bool samePhase){
    if(primary) {
      if(samePhase) {
        --location;
      } else {
        ++location;
      }
    } else {
      if(samePhase) {
        ++location;
      } else {
        --location;
      }
    }
  } /* step */

  /** when cruising we only look at one phase, and possibly one edge.
   * the dirstep records the direction and if one edge then set to 4, if both edges set to 2.
   * One should only cruise when one is sure that the direction cannot change, presumably due to inertia.
   */
  void cruiseDirection(int dirstep){
    this->dirstep = dirstep;
  }

  /** call this with each primary edge when cruising */
  void cruiseStep(){
    location += dirstep;
  }

  /** when using a hardware counter we retain the cruiseDirection setting and report the merge of the last incremental update with the count value.*/
  int fly(unsigned count) const {
    return location + dirstep * count;
  }

  /** when a 16 bit hardware counter wraps we need to update our internal value (unless we keep a rollover count with the counter).
   * Also if we reset the hardware counter such as with an index mark, we need to accumulate the pre-cleared value.
   * Unless the index is an absolute clear. */
  void rollover(unsigned count){
    location += dirstep * count;
  }

  /** reset only applies to location, not to any modality of the counting. */
  void reset(){
    location=0;
  }

} ;
#endif // QUADRATURECOUNTER_H
