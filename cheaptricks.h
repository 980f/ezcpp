#ifndef CHEAPTRICKS_H
#define CHEAPTRICKS_H
#include "eztypes.h"

/** @returns whether @param flag is in the set @param flags. */
bool isPresent(const char *flags, const char flag);


/** compares @param target to @param newvalue, @returns whether the previous value was different and if so assigns that value to the target.
   the goal is for this to be an atomic operation on any truly multi-threaded system. */

template <typename Scalar, typename Scalar2> bool changed(Scalar&target, Scalar2 &&newvalue) {
  if (target != newvalue) {
    target = newvalue;
    return true;
  } else {
    return false;
  }
}

/** assigns newvlaue to target and returns -1,0, or 1 according to whether target was reduced,unchanged, or increased  */

template <typename Scalar, typename Scalar2> int assigncmp(Scalar&target, Scalar2 &&newvalue) {
  if (target != newvalue) {
  	bool reduced = target > newvalue;
    target = newvalue;
    return reduced?-1:1;
  } else {
    return 0;
  }
}

/** @see template of same name. This instantiation does an 'nearly equal' compare */
bool changed(double&target, double &&newvalue);


/** form of AssignOnExit for use in a return statement: */
template <typename Scalar> Scalar postAssign(Scalar&varb, Scalar value) {
  Scalar was = varb;
  varb = value;
  return was;
}

/** form of AssignOnExit for use in a return statement: */
template <typename Scalar,typename Similar> Scalar postAssign(Scalar&varb, Similar value) {
  return postAssign(varb,Scalar(value));
}

/** @return value, zeroes the source */
template <typename Scalar> Scalar take(Scalar&varb) {
  return postAssign(varb,0);
}

/** Clear On Reference. Like core memory :) */
template <typename Scalar> class COR {
  Scalar memory;
  public:
  COR(Scalar init):memory(init){
    //#done
  }

  operator Scalar(){
    return take(memory);
  }

  Scalar operator =(Scalar value){
    memory = value;
    return value;
  }

  ///////////////////////////////////////
  // add non taking operators at need.
  
  /** @returns whether value is not zero, without taking it */
  operator bool()const {
    return memory!=0;
  }
  
  /** added for clirp class */
  bool operator==(Scalar other)const {
    return memory == other;
  }
	
}; //end of COR.

///////////////////////////////////////////////////////
/** for test and clear, may eventually wrap atomic operation */
template <typename Scalar> Scalar flagged(Scalar&varb) ISRISH;
template <typename Scalar> Scalar flagged(Scalar&varb) {
  Scalar was = varb;
  varb = 0;
  return was;
}

//custom instantiation for breakpointing on boolean change.
inline bool flagged(bool &varb) {
  if (varb) {
    varb = 0;
    return true;
  } else {
    return false;
  }
}

/** if @param varb is false sets it to true and @returns true else return false.*/
inline bool notAlready(bool &varb) {
  if (!varb) {
    varb = true;
    return true;
  } else {
    return false;
  }
}

#endif // CHEAPTRICKS_H
