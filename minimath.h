#pragma once
//without the following define the cmath blows us out of the build, it requires some concept of "hosted"
#ifndef __STDC_HOSTED__
#define __STDC_HOSTED__ 1
#endif

#include <cmath>
#include <eztypes.h>

//portable nan etc. symbols, our compilers don't seem to agree on these guys, or the syntax is horrible.
const extern double Infinity;
const extern double Nan;

/** specifically nan, not infinity, @see isSignal() */
bool isNan(double d);

/** isNan(int) exists to allow templates to use isNan for floats without fancy type testing*/
inline bool isNan(int){
  return false;
}

/** maydo: use our ~0 convention here */
inline bool isNan(unsigned){
  return false;
}

/** is a normalized fp number, excludes zero and signals */
bool isNormal(double d);
/** is not a value */
bool isSignal(double d);

/** is either 0 or not a nan. */
bool isDecent(double d);

/** Note: 'signbit' is a macro in math.h that pertains only to floating point arguments
 * @returns sign of operand, and converts operand to its magnitude, MININT(0x800...) is still MININT and must be interpreted as unsigned to work correctly
*/
template <typename SignedInt> int signabs(SignedInt& absolutatus) {
  if (absolutatus < 0) {
    absolutatus = -absolutatus;
    return -1;
  }
  return absolutatus ? 1 : 0;
}

/** Note: 'signbit' is a macro in math.h that pertains only to floating point arguments
 * @returns sign of operand, and converts operand to its magnitude, MININT(0x800...) is still MININT and must be interpreted as unsigned to work correctly
 */
template<typename Numerical> Numerical absvalue(Numerical absolutatus, int *sign = nullptr){
  if(absolutatus < 0) {
    if(sign) {
      *sign = -1;
    }
    return -absolutatus;
  }
  if(sign) {
    *sign = absolutatus ? 1 : 0;
  }
  return absolutatus;
} // absvalue

//yet another filter to reconcile platform math.h issues. Make specializations per platform for performance.
template <typename mathy> int signof(mathy x) {
  if (x < 0) {
    return -1;
  }
  if(x != 0) {//using != instead of > makes NaN's positive instead of 0
    return +1;
  }
  return 0;
}

/** @returns sign of x-y */
template <typename mathy> int signof(mathy x,mathy y) {
  if (x < y) {
    return -1;
  }
  if (x > y) {
    return +1;
  }
  return 0;
}

/** @returns positivity as a multiplier */
inline int polarity(bool positive){
  return positive ? 1 : -1;
}

/** @returns -1 if @param lhs is < @param rhs, 0 if lhs==rhs, +1 if lhs>rhs .
 *  If you are using this to sort ascending if returns + then move lhs to higher than rhs.
 *  Someday this will use <=> but we aren't at that level of C++ for all processors that this code runs on.
 */
template< typename mathy > int compareof(mathy lhs,mathy rhs){
  return signof(lhs - rhs);
}

/** 'round to nearest' ratio of integers*/
inline uint32_t rate(uint32_t num, uint32_t denom) {
  if (denom == 0) {
    return num == 0 ? 1 : 0; //pathological case
  }
  return (num + (denom / 2)) / denom;
}

inline unsigned half(unsigned sum){
  return (sum + 1) / 2;
}

//#rate() function takes unsigned which blows hard when have negative numbers
inline int half(int sum) {
  if(sum<0) {//truncate towards larger magnitude
    return -half(-sum);//probably gratuitous but we also shouldn't be calling this with negatives so we can breakpoint here to detect that.
  }
  return (sum + 1) / 2;
}

/** @returns quantity of bins needed to hold @param num items at @param denom items per bin.
 * If denom is 0 you get back either 1 or 0 depending upon num!=0. This makes sense for the most commonly encountered pathological cases. */
template<typename Integer,typename Inttoo> Integer quanta(Integer num, Inttoo denom){
  //todo:0 insist on unsigned types.
  if (denom == 0) {
    return num == 0 ? 1 : 0; //pathological case
  }
  //todo:0 is typeof num is signed do what 'half' does.
  return (num + denom - 1) / denom;
}

/** @returns @param num / @param denom, protecting against garbage in (divide by zero) note: 0/0 is 0, not 1
 * If you know you are using integers then consider using @see rateof @see quanta instead, one of those is more likely to be what you want.
 */
template <typename NumType,typename DenType=NumType>  double ratio(NumType num, DenType denom) {
  if (denom == 0) { //pathological case
    return num; //attempt to make 0/0 be 1 gave 1 for ratio of unmeasured items, was not a good choice  may someday return signed inf.
  }
  return num / denom;
}

//will eventually split out this section as the goal of  minimath was to NOT include math.h :P
//#include "math.h"//ceil,power,fabs,frexp

inline bool isSignal(double d) {
  return d == Nan || std::isnan(d);
}

/** quantity of bins needed to hold num items at denom items per bin. @see quanta */
inline uint32_t chunks(double num, double denom) {
  double _ratio = ratio(num, denom);

  if (_ratio >= 0) {
    return uint32_t(ceil(_ratio));
  } else {
    return 0;
  }
}

/** round to a quantum, to kill trivial trailing DECIMAL digits*/
inline double rounder(double value, double quantum) {
  return quantum * chunks(value, quantum);
}

/** @returns canonical value % cycle, minimum positive value
 *  0<= return <cycle;
 *  Note: the C '%' operator gives negative out for negative in.
*/
int modulus(int value, unsigned cycle);


/** standard math lib's f_r_exp does a stupid thing for some args, we wrap it here and fix that.*/
inline int fexp(double d) ISRISH;
inline int fexp(double d) {
  int ret;
  if (d == 0.0) { //frexp returns 0, which makes it look bigger than numbers between 0 and 1.
    return -1023;//this value is one less than any non-zero number will give (this constant is ieee64 specific)
  }
  frexp(d, &ret);
  return ret;
}

/** @returns whether the difference of the two numbers is less than a power of two (which defaults to 2^-14) times the bigger of the two. */
template <typename floating> bool nearly(floating value, floating other, int bits = 14) {
  floating diff = value - other;
  if (diff == 0.0) { //frequent case, and handles many pathologies as well
    return true;
  }
  int f1 = fexp(value);
  int f2 = fexp(other);
  //if either is zero absolute compare the other to 2^-bits;
  if (value == 0.0) { //fexp on 0 isn't sanitizable.
    return (f2 + bits) < 0;
  }
  if (other == 0.0) {
    return (f1 + bits) < 0;
  }
  int cf = fexp(diff);
  cf += bits;
  return cf <= f1 && cf <= f2;
}

/** @returns the integer part of the log base 10 of @param value.
   That is the number of decimal digits to the left of the radix point */
int ilog10(uint32_t value);

/** filtering in case we choose to optimize this */
inline double pow10(int exponent) {
  return pow(10.0D, exponent);
}

template <typename mathy> mathy squared(mathy x) {
  return x * x;
}

unsigned removeTrailingZeroes(unsigned wantodd) WEAK ; //weak because some processors have an instruction for this.


/** n!/r! = n*(n-1)..*(n-r+1)
*/
uint32_t Pnr(unsigned n, unsigned  r);
/** n!/(r!(n-r)!)*/
uint32_t Cnr(unsigned n, unsigned  r);


/** if a is greater than b set it to b and @return whether a change was made.
 *  if orequal is true then also return true if args are equal.
 *  if a is Nan then do the assign and return true */
template< typename S1, typename S2 > bool depress(S1 &a, S2 b,bool orequal = false){
  if(isNan(b)) {
    return false;
  }
  S1 b1 = S1(b); //so incomparable types gives us just one error.
  if(isNan(a) || a > b1) {
    a = b1;
    return true;
  }
  return orequal && a==b1;
} // depress

/** if a is less than b set it to b and @return whether a change was made.
 *  if orequal is true then also return true if args are equal.
 *  if a is Nan then do the assign and return true */
template< typename S1, typename S2 > bool elevate(S1 &a, S2 b,bool orequal = false){
  if(isNan(b)) {
    return false;
  }
  S1 b1 = S1(b); //so incomparable types gives us just one error.
  if(isNan(a) || a < b1) {
    a = b1;
    return true;
  }
  return orequal && a==b1;
} // elevate


////#define something as needed to kill any other min/max's as needed.
//template adandoned as the firmware rev of gcc couldn't deal with uint32_t vs unsigned int, i.e. it type checked  before applying typedef's
//template <typename Scalar> Scalar min(Scalar a,Scalar b){
//  if(a<b){
//    return a;
//  } else {
//    return b;
//  }
//}

//using 'lesser' and 'greater' while we check if all of our compilers now have compatible min and max std functions.

//todo:2 see if compiler can use this for min of convertible types:
template <typename S1, typename S2> S1 lesser(S1 a, S2 b) {
  S1 b1 = static_cast<S1>(b); //so incomparable types gives us just one error.
  if (a < b1) {
    return a;
  } else {
    return b1;
  }
}

template< typename S1, typename S2 > S1 greater(S1 a, S2 b){
  S1 bb = S1(b);
  if(a > bb) {
    return a;
  } else {
    return bb;
  }
}


#undef max
template <typename Scalar, typename S2> Scalar max(Scalar a, S2 b) {
  Scalar bb = static_cast<Scalar>(b); //so incomparable types gives us just one error.
  if (a > bb) {
    return a;
  } else {
    return bb;
  }
}

template <typename Scalar> void swap(Scalar &a, Scalar &b) {
  Scalar noxor;//don't trust that xor is non corrupting for all scalars.
  noxor = a;
  a = b;
  b = noxor;
}

extern "C" { //assembly coded in cortexm3.s, usually due to outrageously bad compilation by gcc
  /** the time delay given by ticks is ambiguous, it depends upon processor clock. For STM32F103 @72MHz 1000 ticks is roughly one microsecond.*/
  void nanoSpin(unsigned ticks); //fast spinner, first used in soft I2C.

  /** @returns @param arg*num/denom rounded and overflow managed (internal 64 bit temps)  */
  uint32_t muldivide(uint32_t arg, uint32_t num, uint32_t denom);

  uint16_t saturated(unsigned quantity, double fractionThereof);

  //fraction is a fractional multiplier, with numbits stating how many fractional bits it has.
  uint16_t fractionallyScale(uint16_t number, uint16_t fraction, uint16_t numbits);

  /** this has waffled about a bit, differing by one in its return for various vintages.
     @returns value such that 1<<return is >= @param number.
     want 1->0, 0->-1, 2^n->n (2^n + 2^k)->n
      Note well that this will give -1 as the log of 0 rather than negative infinity, precheck the argument if you can't live with that.
  */
  int log2Exponent(uint32_t number);

  /** @returns eff * 2^pow2  where pow2 is signed. This can be done rapidly via bitfiddling*/
  float shiftScale(float eff, int pow2);

  double flog(uint32_t number);
  double logRatio(uint32_t over, uint32_t under);

  uint16_t uround(float scaled);
  int16_t sround(float scaled);

  /**NB: copyObject() and fillObject() can NOT be used with objects that contain polymorphic objects as they would copy the virtual function tables */
  void copyObject(const void *source, void *target, uint32_t length);
  void fillObject(void *target, uint32_t length, uint8_t  fill);

  //EraseThing only works on non-polymorphic types. On polymorphs it also  kills the vtable!
#define EraseThing(thing) fillObject(thing, sizeof(thing), 0);

  /** note different order (and type!) of args than std C library memcpy. */
  void memory_copy(const void *source, void *target, void *sourceEnd);
  /** @see memory_copy */
  void memory_set(void *target, void *targetEnd, uint8_t  value);

#if 0 //  fixmelater //!defined( QT_CORE_LIB ) && !defined() //std lib's differ between pc and arm.
  //the difference of two uint16_t's should be a signed int. test your compiler.
  inline uint16_t abs(int value) {
    return value > 0 ? value : -value;
  }
#endif

} //end extern C for assembly coded routines.

//compile time math:

/** @returns exponent that is highest power of 2 in the given number */
template < unsigned powerof2, int count = -1 > struct lb {
  enum {
    exponent =  (powerof2 > 0) ? lb < powerof2 / 2, count + 1 >::exponent : count
  };
};

/** this was needed as the compiler insisted on evaluating the 'true' term of the ternary even when its conditional was false */
template<int count> struct lb<0, count> {
  enum {
    exponent = count
  };
};

constexpr unsigned lb2(unsigned number) {
  return number ? 1 + lb2(number >> 1) : 0;
}
