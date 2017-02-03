#include "minimath.h"

#if 0 && __linux__
#include <limits>
const double Infinity = std::numeric_limits<double>::infinity();
const double Nan = std::numeric_limits<double>::quiet_NaN();
#else
//firmware platform didn't have a useful limits.h so ...
union punter{ long long i64; double d;};
const double Infinity=punter{0x7FFLL<<52}.d;
const double Nan=     punter{0x7FF8LL<<48}.d;

union pfunter{ unsigned i32; float f;};
const float fInfinity=pfunter{0xFFU<<27}.f;
const float fNan=     pfunter{0xFF8U<<23}.f;

#endif

extern "C" {
//stubs.cpp keeps on getting lost, inline those here weakly:

/** @returns @param arg*num/denom rounded and overflow managed (internal 64 bit temps)  */
u32 muldivide(u32 arg, u32 num, u32 denom) WEAK;
u32 muldivide(u32 arg, u32 num, u32 denom){
  return rate(arg*num,denom);
}

int log2Exponent(u32 number) WEAK;
int log2Exponent(u32 number){
  int exponent=-1;
  while(number){
    ++exponent;
    number>>=1;
  }
  return exponent;
}

}//end stubs

const u32 Decimal1[] = {
  1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000,
};

/** @returns the number of decimal digits needed to represent the given integer, -1 if the number is 0 */
int ilog10(u32 value){
  for(int log=countof(Decimal1);log-->0;){
    if(Decimal1[log]<=value) {
      return log;
    }
  }
  return -1;
}

//uround and sround are coded to be like they will in optimized assembly
u16 uround(float scaled){
  if(scaled < 0.5) { //fp compares are the same cost as integer.
    return 0;
  }
  scaled *= 2; //expose rounding bit
  if(scaled >= 131071) {
    return 65535;
  }
  int eye = int(scaled); //truncate
  return u16(eye / 2);
} /* uround */

s16 sround(float scaled){ //#this would be so much cleaner and faster in asm!
  if(scaled > 32766.5) {
    return 32767;
  }
  if(scaled < -32767.5) {
    return -32768;
  }
  scaled += scaled >= 0 ? 0.5 : -0.5; //round away from 0. aka round the magnitude.
  return int(scaled);
}

int modulus(int value, unsigned cycle){
  /* since most use cases are within one cycle we use add/sub rather than try to make divide work.*/
  if(cycle<=1){
    return value;
  }
  while(value < 0) {
    value += cycle;
  }
  while(unsigned(value) >= cycle) {
    value -= cycle;
  }
  return value;
}

u16 saturated(unsigned quantity, double fractionThereof){
  unsigned rawbins = quantity * fractionThereof;//todo:2 is truncating rather than rounding

  if(rawbins >= quantity) {
    return quantity - 1;
  } else {
    return rawbins;
  }
} /* saturated */


#undef __STRICT_ANSI__
#include <math.h>

#define gotFlogWorking 0

#if gotFlogWorking == 2

static double LN2 = 0.69314718055994530942;
double flog(u32 number){
  int exponent = log2Exponent(number);
  int malign = number << (30 - exponent); //unsigned 1.31

  malign = 1<<31 - malign; //
  //goose until x is less than 1/2
  return -1.0; //not yet implemented
}

#elif gotFlogWorking == 1
static double LN2 = 0.69314718055994530942;
double flog(u32 number){

  static u32 fractroots[] = { //fractional part of the roots of 2
    0x6A09E667,
    0x306FE0A3,
    0x172B83C7,
    0xB5586CF,
    0x59B0D31,
    0x2C9A3E7,
    0x163DA9F,
    0xB1AFA5,
    0x58C86D,
    0x2C605E,
    0x162F39,
    0xB175E,
    0x58BA0,
    0x2C5CC,
    0x162E5,
    0xB172,
    0x58B9,
    0x2C5C,
    0x162E,
    0xB17,
    0x58B,
    0x2C5,
    0x162,
    0xB1,
    0x58,
    0x2C,
    0x16,
    0xB,
    0x5,
    0x2,
    0x1,
  };
  int exponent = log2Exponent(number);
  int malign = number << (31 - exponent); //actually is unsigned 1.31

  malign &= ~(1 << 31); //same as subtracting 1 in 1.31 format
  //malign is now a 0.31 format number, in the sub range of 0 to 1-2^-31.
  unsigned n = countof(fractroots);
  u32 logish = 0;
  u32 checker = 0; //we will iterate to make checker close to malign
  u32 *fract = fractroots;

  while(n-- > 0) { //#order matters
    u64 tester = *fract * checker; //best done in asm
    tester += u64(*fract + checker) << 32;
    if(tester <= u64(malign) << 32) {
      checker = tester >> 32;
      logish |= 1 << (n); //maybe n+1?
    }
    ++fract;
  }
  //logish is mantissa of base 2 log
  //pack fields
  u64 packer = u64(exponent) << 52;
  packer |= logish << (52 - 32);
  return pun(double, packer) * LN2; //<math.h>::M_LN2 log base e of 2.
} /* flog */
#else /* if gotFlogWorking == 2 */
double flog(u32 number){
  if(number == 0) {
    number = 1;
  }
  return log(double(number)); //without the cast the gcc-arm compiler just called __floatunsidf
}

#endif /* if gotFlogWorking == 2 */

#if logoptimized
double logRatio(u32 over, u32 under){
  unsigned underexp = 32 - log2Exponent(under); //leading zeroes
  unsigned overexp = 32 - log2Exponent(over); //leading zeroes
  int diff = overexp - underexp;
  bool negate = diff < 0;

  if(negate) { ///then ratio is greater than 1
    diff = -diff;
    under <<= diff;
  } else {
    over <<= diff;
    //swap over and under:
  }
  //could also trim leading zeroes of over.
  over = under - over; //effectively convert x to 1-y and then iterate over y.
  //now do simple series expansion using ratio powers.

} /* logRatio */
#else /* if logoptimized */
//someday we will optimize the following:
double logRatio(u32 over, u32 under){
  return flog(over) - flog(under);
}
#endif /* if logoptimized */

/** n!/r! = n*(n-1)..*(n-r+1) */
u32 Pnr(unsigned n, unsigned  r){
  if(r<=0){//frequent case and avert naive divide by zero
    return 1;
  }
  if(r==1){//fairly frequent case
    return n;
  }
  u32 num=n;
  while(n-->r){//n!/(n-r)!
    num*=n;
  }
  return num;
}

///** n!/r!(n-r)! = n*(n-1)..*(n-r+1) / r! */
u32 CnrSimple(unsigned n, unsigned  r){
  if(r<=0){//frequent case and avert naive divide by zero
    return 1;
  }
  if(r==1){//fairly frequent case
    return n;
  }
  u32 num=Pnr(n,r);
  u32 den=r;
  while(r-->2){
    den*=r;
  }
  return num/den;//#zero is checked
}



/** n!/r!(n-r)! = n*(n-1)..*(n-r+1)/r*(r-1)..
This is done in a complicated fashion to increase the range over what could be done if the factorials were computed then divided.
*/
u32 Cnr(unsigned n, unsigned  r){
  if(r<=0){//frequent case and avert naive divide by zero
    return 1;
  }
  if(r==1){//fairly frequent case
    return n;
  }
  if(r==2){
    //divide the even number by 2, via shift.
    if(n&1){
      return n*((n-1)>>1);
    } else {
      return (n>>1)*(n-1);
    }
  }

  u32 num=n;
  u32 denom=r;
  //optimize range by removing power of 2 from factorials while computing them
  int twos=0;
  while(r-->0){
    unsigned nterm=--n;
    while(0==(nterm&1)){//50% of the time we loop just once, 25% of the time twice 12.5% of the time 3 times ...
      ++twos;
      nterm>>=1;
    }
    num*=nterm;
    unsigned rterm=r;
    while(0==(rterm&1)){
      --twos;//these discarded twos are in the denominator
      rterm>>=1;
    }
    denom*=rterm;
  }
  //twos should be a small
  if(twos>=0){
    num<<=twos;
  } else {
    denom<<=-twos;
  }
  return rate(num,denom);
}
