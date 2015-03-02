#ifndef minimath_h
#define minimath_h
#include "eztypes.h"

// portable nan etc. symbols, our compilers don't seem to agree on these guys, or the syntax is horrible.
const extern double Infinity;
const extern double Nan;


/** 'signbit' is a macro in math.h that pertains only to floating point arguments
 * @return sign of operand, and convert operand to its magnitude, MININT(0x800...) is still MININT and must be interpreted as unsigned to work correctly
 */
inline int signabs(int &absolutatus){
  if(absolutatus < 0) {
    absolutatus = -absolutatus;
    return -1;
  }
  return absolutatus ? 1 : 0;
}

inline int signum(int anint){
  if(anint < 0) {
    return -1;
  }
  return anint ? 1 : 0;
}

// yet another filter to reconcile platform math.h issues. Make specilaizations per platform for performance.
template <typename mathy> int signof(mathy x){
  if(x < 0) {
    return -1;
  }
  if(x > 0) {
    return +1;
  }
  return 0;
}


/** 'round to nearest' ratio of integers*/
inline u32 rate(u32 num, u32 denom){
  if(denom == 0) {
    return num == 0 ? 1 : 0; // pathological case
  }
  return (num + (denom / 2)) / denom;
}

// #rate() function takes unsigned which blows hard when have negative numbers
inline int half(int sum){
  return (sum + 1) / 2;
}

/** quantity of bins needed to hold num items at denom items per bin*/
inline u32 quanta(u32 num, u32 denom){
  if(denom == 0) {
    return num == 0 ? 1 : 0; // pathological case
  }
  return (num + denom - 1) / denom;
}

/** protect against garbage in (divide by zero) note: 0/0 is 0*/
inline double ratio(double num, double denom){
  if(denom == 0) { // pathological case
    return Nan; // 0/0 == 0, not 1.
  }
  return num / denom;
}

/** protect against garbage in (divide by zero) note: 0/0 is 0*/
inline float ratio(float num, float denom){
  if(denom == 0) { // pathological case
    return Nan;
  }
  return num / denom;
}

// will eventually split out this section as the goal of  minimath was to NOT include math.h :P
#include "math.h" // ceil,power,fabs,frexp

inline bool isSignal(double d){
  return d == Nan || isnan(d);
}

/** quantity of bins needed to hold num items at denom items per bin*/
inline u32 chunks(double num, double denom){
  double _ratio = ratio(num, denom);

  if(_ratio >= 0) {
    return u32(ceil(_ratio));
  } else {
    return 0;
  }
}

/** round to a quantum, to kill trivial trailing DECIMAL digits*/
inline double rounder(double value, double quantum){
  return quantum * chunks(value, quantum);
}

/** @returns canonical @param value modulo &param cycle, the minimum positive value such that
 *  0<= return <cycle;
 *  % operator gives negative out for negative in.
 */
int modulus(int value, unsigned cycle);

/** standard math lib's f_r_exp does a stupid thing for some args, we wrap it here and fix that.*/
inline int fexp(double d) ISRISH;
inline int fexp(double d){
  if(d == 0.0) { // frexp returns 0, which makes it look bigger than numbers between 0 and 1.
    return -1023; // one less than any non-zero number will give
  } else {
    int ret;
    frexp(d, &ret);
    return ret;
  }
}

/** @returns whether the difference of the two numbers is less than a power of two (which defaults to 2^-14) times the bigger of the two. */
template <typename floating> bool nearly(floating value, floating other, int bits = 14){
  floating diff = value - other;

  if(diff == 0.0) { // frequent case, and handles many pathologies as well
    return true;
  }
  int f1 = fexp(value);
  int f2 = fexp(other);
  // if either is zero absolute compare the other to 2^-bits;
  if(value == 0.0) { // fexp on 0 isn't sanitizable.
    return f2 + bits < 0;
  }
  if(other == 0.0) {
    return f1 + bits < 0;
  }
  int cf = fexp(diff);
  cf += bits;
  return cf <= f1 && cf <= f2;
} // nearly

/** table of small powers of ten */
extern const u32 Decimal1[];

/** @returns number of decimal digits needed to represent @param value, unless value is zero in which case it returns @param forzero.
 the default value of 1 for when given a value of 0 is handy when figuring out how many characters are needed to print a number. */
int ilog10(u32 value, int forzero=1);

/** filtering in case we choose to optimize this */
inline double pow10(int exponent);

template <typename mathy> mathy squared(mathy x){
  return x * x;
}

inline u32 min(u32 a, u32 b){
  if(a < b) {
    return a;
  } else {
    return b;
  }
}
// todo: see if compiler can use this for min of convertible types:
template <typename S1, typename S2> S1 lesser(S1 a, S2 b){
  S1 b1 = S1(b); // so incomparable types gives us just one error.

  if(a < b1) {
    return a;
  } else {
    return b1;
  }
}

template <typename Scalar, typename S2> Scalar max(Scalar a, S2 b){
  Scalar bb = Scalar(b);

  if(a > bb) {
    return a;
  } else {
    return bb;
  }
}

template <typename Scalar> void swap(Scalar &a, Scalar &b){
  Scalar noxor; // don't trust that xor is non corrupting for all scalars.
  noxor = a;
  a = b;
  b = noxor;
}

extern "C" { // assembly coded in cortexm3.s, usually due to outrageously bad compilation by gcc
  /** delay for at least @param nanos nanoseconds. @param nanosResolution is something that you will have to tune for your platform. */
  void nanoSpin(unsigned nanos,unsigned nanosResolution=83);

  // rounded and overflow managed 'multiply by ratio'
  u32 muldivide(u32 arg, u32 num, u32 denom);

  u16 saturated(unsigned quantity, double fractionThereof);

  // fraction is a fractional multiplier, with numbits stating how many fractional bits it has.
  u16 fractionallyScale(u16 number, u16 fraction, u16 numbits);
  /** 1 + the integer part of log base 2 of the given number, pretty much is just "count the leading zeroes".
   * Note well that this will give 0 as the log of 0 rather than negative infinity, precheck the argument if you can't live with that.
   * mathematical definition: "number of right shifts necessary for an unsigned number to become 0"
   */
  u32 log2Exponent(u32 number);

  /** return eff * 2^pow2  where pow2 is signed. This can be done rapidly via bitfiddling*/
  float shiftScale(float eff, int pow2);

  /** @returns the logarithm of @param number if the number>0 else returns 0. */
  double flog(u32 number);

  /** @returns the logarithm of the ratio of @param over/ @param under except if either is 0 they are treated instead as 1.*/
  double logRatio(u32 over, u32 under);

  /** @returns 65536*@param scaled, except scaled>=.9999 gives 65535 */
  u16 uround(float scaled);

  /** @returns 32768*scaled, with -32768 for scaled <=-0.5, 32767 scaled >=0.499999 */
  s16 sround(float scaled);

  /**NB: copyObject() and fillObject() can NOT be used with objects that contain polymorphic objects*/
  /** wrappers for memcpy, allowing platform specific optimizations */
  void copyObject(const void *source, void *target, u32 length);
  /** wrappers for memset, allowing platform specific optimizations */
  void fillObject(void *target, u32 length, u8 fill);

  // EraseThing only works on non-polymorphic types. On polymorphs it also  kills the vtable!
#define EraseThing(thing) fillObject(thing, sizeof(thing), 0);
} // end extern C for (potentially) assembly coded routines.

#endif /* ifndef minimath_h */
