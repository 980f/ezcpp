#ifndef CHEAPTRICKS_H
#define CHEAPTRICKS_H
#include "eztypes.h"

/** @returns whether @param flag is in the set @param flags. */
bool isPresent(const char *flags, const char flag);


/** compares @param target to @param newvalue, @returns whether the previous value was different and if so assigns that value to the target.
 * the goal is for this to be an atomic operation on any truly multi-threaded system. */

template <typename Scalar> bool changed(Scalar&target, Scalar newvalue){
  if(target != newvalue) {
    target = newvalue;
    return true;
  } else {
    return false;
  }
}

/** @see template of same name. This instantiation does an 'nearly equal' compare */
bool changed(double&target, double newvalue);

/** clear a variable on block exit, regardless of how the exit happens, including exceptions */
template <typename Scalar> class ClearOnExit {
  Scalar&zipperatus;
public:
  ClearOnExit(Scalar & toBeCleared): zipperatus(toBeCleared){}
  operator Scalar(void){
    return zipperatus;
  }

  ~ClearOnExit (){
    zipperatus = 0;
  }
};


/** assign a value to variable on block exit, regardless of how the exit happens, including exceptions.
  * NB: records value to use at time of this object's creation */
template <typename Scalar> class AssignOnExit {
  Scalar&zipperatus;
  Scalar onexit;
public:
  AssignOnExit(Scalar & toBeCleared, Scalar onexit): zipperatus(toBeCleared), onexit(onexit){}
  ~AssignOnExit (){
    zipperatus = onexit;
  }
  /** access wrapped entity via this object, handy when wrapping a dynamically selected item */
  operator Scalar() const {
    return zipperatus;
  }
  /** @returns eventual value - present value , the change that will be imposed at exit */
  Scalar delta(void){
    return onexit - zipperatus;
  }
};

/** form of AssignOnExit for use in a return statement: */
template <typename Scalar> Scalar postAssign(Scalar&varb, Scalar value){
  Scalar was = varb;
  varb = value;
  return was;
}


/** for test and clear, may eventually wrap atomic operation */
template <typename Scalar> Scalar flagged(Scalar&varb) ISRISH;
template <typename Scalar> Scalar flagged(Scalar&varb) {
  Scalar was = varb;
  varb = 0;
  return was;
}

//custom instantiation for breakpointing on boolean change.
inline bool flagged(bool &varb){
  if(varb){
    varb=0;
    return true;
  } else {
    return false;
  }
}

/** if @param varb is false sets it to true and @returns true else return false.*/
inline bool notAlready(bool &varb){
  if(!varb){
    varb=true;
    return true;
  } else {
    return false;
  }
}

#endif // CHEAPTRICKS_H
