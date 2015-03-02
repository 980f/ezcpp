#include "minimath.h"

#ifdef __linux__
#include <limits>
const double Infinity = std::numeric_limits<double>::infinity();
const double Nan = std::numeric_limits<double>::quiet_NaN();
#else
// firmware platform didn't have a useful limits.h so ...
constexpr int64_t InfPattern = 0x7FFLL << 52;
constexpr int64_t NanPattern = 0x7FF8LL << 48;

const double Infinity = punt<const double>(InfPattern);
const double Nan = punt<const double>(NanPattern);
#endif

const u32 Decimal1[] = {
  1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000,
};

int ilog10(u32 value,int forzero){
  for(int log = countof(Decimal1); log--> 0;) {
    if(Decimal1[log] <= value) {
      return 1+log;
    }
  }
  return forzero;
}


u16 uround(float scaled){
  //bit pattern, using signed as the sign bit is conveniently in the same place.
  int ieee=punt<int>(scaled);

  if(ieee < 0x3f000000){//0.5. also handles negative, 0, and denormed values
    return 0;
  }
  int exponent=(ieee>>23) -127;

  if(exponent>=16){//65534.5
    return 65535;
  }
  //extract mantissa and restore hidden msb.
  unsigned mantissa= ( ieee & ((1<<23)-1)) | (1<<23);
  //round
  mantissa+= 1 << (23-exponent-1); // x-15=7 => x=22

  //if exponent is 15 want to shift right 8 places, if 0 then 23.
  return u16(mantissa>>(23-exponent));
} /* uround */

s16 sround(float scaled){
  //bit pattern, using signed as the sign bit is conveniently in the same place.
  int ieee=punt<int>(scaled);

  if(ieee> 0x46fffd00) {//32766.5
    return 32767;
  }
  if(unsigned(ieee)>0xc6ffff00) {//-32767.5
    return -32768;
  }
  int exponent=(ieee>>23) -127;//the compiler when optimizing folds the -127 into all access of exponent rather than doing it 'now'
  //extract mantissa and restore hidden msb.
  unsigned mantissa= ( ieee & ((1<<23)-1)) | (1<<23);
  //round
  mantissa+= 1 << (23-exponent-1);
  if(ieee<0){
    mantissa=-mantissa;
  }
  return int(mantissa>>(23-exponent));
}

int modulus(int value, unsigned cycle){
  /* since most use cases are within one cycle we use add/sub rather than try to make divide work.*/
  if(cycle <= 1) {
    return value;
  }
  while(value < 0) {
    value += cycle;
  }
  while(unsigned(value) >= cycle) {
    value -= cycle;
  }
  return value;
} // modulus

u16 saturated(unsigned quantity, double fractionThereof){
  unsigned rawbins = quantity * fractionThereof; // todo:2 is truncating rather than rounding

  if(rawbins >= quantity) {
    return quantity - 1;
  } else {
    return rawbins;
  }
} /* saturated */


#undef __STRICT_ANSI__
#include <math.h>

double flog(u32 number){
  if(number == 0) {
    return 0.0;
  }
  return log(double(number)); // without the cast the gcc-arm compiler just called __floatunsidf
}

// someday we will optimize the following:
double logRatio(u32 over, u32 under){
  return flog(over) - flog(under);
}


double pow10(int exponent){
  if(exponent >= 0 && unsigned(exponent)< countof(Decimal1)) {
    return Decimal1[exponent];
  }
  return pow(10, exponent);
}
