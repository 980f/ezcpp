#ifndef CHEAPTRICKS_H
#define CHEAPTRICKS_H
#include "eztypes.h"

/** @returns whether @param flag is in the set @param flags. */
bool isPresent(const char *flags, char flag);

/** @see template of same name */
bool changed(double &target, double newvalue);

/** compares @param target to @param newvalue, @returns whether the previous value was different and if so assigns that value to the target.
 * the goal is for this to be an atomic operation on any truly multi-threaded system. */
// todo: make this weak so that it can be transparently overridden by assembler coded routines.
template <typename Scalar> bool changed(Scalar &target, Scalar newvalue){
  if(target != newvalue) {
    target = newvalue;
    return true;
  } else {
    return false;
  }
}

/** in order to put various otherwise disparate classes into a single array of on/off entities*/
class Boolish {
public:
  virtual operator bool() const =0;
  virtual bool operator =(bool newvalue)=0;
};

/** @returns value of @param varb prior to clearing it herein */
template <typename Scalar> Scalar flagged(Scalar &varb) ISRISH; // ISRISH attribute can't be set in definition for gcc 4.3.3, try again with later versions.

template <typename Scalar> Scalar flagged(Scalar &varb){
  Scalar was = varb;

  varb = 0;
  return was;
}

/** custom instantiation of flagged for booleans. */
inline bool flagged(bool &varb){
  if(varb) {
    varb = 0;
    return true;
  } else {
    return false;
  }
}

/** @returns whether @param varb was false setting it to true (if it was).
 *  converse of @see flagged(bool) */
inline bool notAlready(bool &varb){
  if(! varb) {
    varb = true;
    return true;
  } else {
    return false;
  }
}


/** clear a variable on block exit, regardless of how the exit happens, including exceptions */
template <typename Scalar> class ClearOnExit {
  Scalar &zipperatus;
public:
  ClearOnExit(Scalar &toBeCleared): zipperatus(toBeCleared){
  }
  ~ClearOnExit(){
    zipperatus = 0;
  }
  /** @returns present value of variable to force when scope is exited. This is useful if the address stored herein is not stored elsewhere in the scope, such as when set via a function call in constructor list */
  operator Scalar(void){
    return zipperatus;
  }
};


/** assign a value to variable on block exit, regardless of how the exit happens, including exceptions.
 * NB: records value to use at time of this object's creation */
template <typename Scalar> class AssignOnExit {
  Scalar &zipperatus;
  Scalar onexit;
public:
  AssignOnExit(Scalar &toBeCleared, Scalar onexit): zipperatus(toBeCleared), onexit(onexit){
  }
  ~AssignOnExit(){
    zipperatus = onexit;
  }
  /** @returns present value of variable to force when scope is exited. This is useful if the address stored herein is not stored elsewhere in the scope, such as when set via a function call in constructor list */
  operator Scalar(void){
    return zipperatus;
  }
  /** @returns difference from value that variable will have on exit to its present value.
   * This is useful if that value is computed in the constructor parameter list */
  Scalar delta(void){
    return onexit - zipperatus;
  }
};

/** form of AssignOnExit for use in a return statement. IE used to return the present value of @param varb while setting it to the given @param value */
template <typename Scalar> Scalar postAssign(Scalar &varb, Scalar value){
  Scalar was = varb;

  varb = value;
  return was;
}


#endif // CHEAPTRICKS_H
