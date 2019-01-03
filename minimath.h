#pragma once

#include "eztypes.h"

//portable nan etc. symbols, our compilers don't seem to agree on these guys, or the syntax is horrible.
const extern double Infinity;
const extern double Nan;


/** 'signbit' is a macro in math.h that pertains only to floating point arguments
    @return sign of operand, and convert operand to its magnitude, MININT(0x800...) is still MININT and must be interpreted as unsigned to work correctly
*/
inline int signabs(int&absolutatus) {
  if (absolutatus < 0) {
    absolutatus = -absolutatus;
    return -1;
  }
  return absolutatus ? 1 : 0;
}

//replaced with signof<>. Name was too popular with other math packages.
//inline int signum(int anint) {
//  if (anint < 0) {
//    return -1;
//  }
//  return anint ? 1 : 0;
//}

//yet another filter to reconcile platform math.h issues. Make specializations per platform for performance.
template <typename mathy> int signof(mathy x) {
  if (x < 0) {
    return -1;
  }
  if (x > 0) {
    return +1;
  }
  return 0;
}


/** 'round to nearest' ratio of integers*/
inline u32 rate(u32 num, u32 denom) {
  if (denom == 0) {
    return num == 0 ? 1 : 0; //pathological case
  }
  return (num + (denom / 2)) / denom;
}

//#rate() function takes unsigned which blows hard when have negative numbers
inline int half(int sum) {
  return (sum + 1) / 2;
}

/** quantity of bins needed to hold num items at denom items per bin*/
inline u32 quanta(u32 num, u32 denom) {
  if (denom == 0) {
    return num == 0 ? 1 : 0; //pathological case
  }
  return (num + denom - 1) / denom;
}

/** protect against garbage in (divide by zero) note: 0/0 is 0 */
template <typename NumType,typename DenType=NumType>  double ratio(NumType num, DenType denom) {
  if (denom == 0) { //pathological case
    return num; //attempt to make 0/0 be 1 gave 1 for ratio of unmeasured items, was not a good choice  may someday return signed inf.
  }
  return num / denom;
}

//will eventually split out this section as the goal of  minimath was to NOT include math.h :P
#include "math.h"//ceil,power,fabs,frexp

inline bool isSignal(double d) {
  return d == Nan || isnan(d);
}

/** quantity of bins needed to hold num items at denom items per bin. @see quanta */
inline u32 chunks(double num, double denom) {
  double _ratio = ratio(num, denom);

  if (_ratio >= 0) {
    return u32(ceil(_ratio));
  } else {
    return 0;
  }
}

/** round to a quantum, to kill trivial trailing DECIMAL digits*/
inline double rounder(double value, double quantum) {
  return quantum * chunks(value, quantum);
}

/** canonical value % cycle, minimum positive value
  0<= return <cycle;
  % operator gives negative out for negative in.
*/
int modulus(int value, unsigned cycle);


/** standard math lib's f_r_exp does a stupid thing for some args, we wrap it here and fix that.*/
inline int fexp(double d) ISRISH;
inline int fexp(double d) {
  int ret;
  if (d == 0.0) { //frexp returns 0, which makes it look bigger than numbers between 0 and 1.
    return -1023;//one less than any non-zero number will give (this constant is ieee64 specific)
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
int ilog10(u32 value);

/** filtering in case we choose to optimize this */
inline double pow10(int exponent) {
  return pow(10.0D, exponent);
}

template <typename mathy> mathy squared(mathy x) {
  return x * x;
}

/** n!/r! = n*(n-1)..*(n-r+1)
*/
u32 Pnr(unsigned n, unsigned  r);
u32 Cnr(unsigned n, unsigned  r);



////#define something as needed to kill any other min/max's as needed.
//template adandoned as the firmware rev of gcc couldn't deal with u32 vs unsigned int, i.e. it type checked  before applying typedef's
//template <typename Scalar> Scalar min(Scalar a,Scalar b){
//  if(a<b){
//    return a;
//  } else {
//    return b;
//  }
//}

//todo: see if compiler can use this for min of convertible types:
template <typename S1, typename S2> S1 lesser(S1 a, S2 b) {
  S1 b1 = static_cast<S1>(b); //so incomparable types gives us just one error.
  if (a < b1) {
    return a;
  } else {
    return b1;
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
  /** the time delay given by ticks is ambiguous, it depends upon processor clock. @72MHz 1000 ticks is roughly one microsecond.*/
  void nanoSpin(unsigned ticks); //fast spinner, first used in soft I2C.

  /** @returns @param arg*num/denom rounded and overflow managed (internal 64 bit temps)  */
  u32 muldivide(u32 arg, u32 num, u32 denom);

  u16 saturated(unsigned quantity, double fractionThereof);

  //fraction is a fractional multiplier, with numbits stating how many fractional bits it has.
  u16 fractionallyScale(u16 number, u16 fraction, u16 numbits);

  /** this has waffled about a bit, differing by one in its return for various vintages.
     @returns value such that 1<<return is >= @param number.
     want 1->0, 0->-1, 2^n->n (2^n + 2^k)->n
      Note well that this will give -1 as the log of 0 rather than negative infinity, precheck the argument if you can't live with that.
  */
  int log2Exponent(u32 number);

  /** @returns eff * 2^pow2  where pow2 is signed. This can be done rapidly via bitfiddling*/
  float shiftScale(float eff, int pow2);

  double flog(u32 number);
  double logRatio(u32 over, u32 under);

  u16 uround(float scaled);
  s16 sround(float scaled);

  /**NB: copyObject() and fillObject() can NOT be used with objects that contain polymorphic objects as they would copy the virtual function tables */
  void copyObject(const void *source, void *target, u32 length);
  void fillObject(void *target, u32 length, u8 fill);

  //EraseThing only works on non-polymorphic types. On polymorphs it also  kills the vtable!
#define EraseThing(thing) fillObject(thing, sizeof(thing), 0);

  /** note different order (and type!) of args than std C library memcpy. */
  void memory_copy(const void *source, void *target, void *sourceEnd);
  /** @see memory_copy */
  void memory_set(void *target, void *targetEnd, u8 value);

#if 0 //  fixmelater //!defined( QT_CORE_LIB ) && !defined() //std lib's differ between pc and arm.
  //the difference of two u16's should be a signed int. test your compiler.
  inline u16 abs(int value) {
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
