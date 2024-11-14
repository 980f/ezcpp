#include "twiddler.h"
#include "minimath.h"  //ratio and quanta
#include <limits>

////a value used to get the maximum precision/resolution available due to the data type of twiddle
//const unsigned MaxRes = std::numeric_limits<decltype(MaxRes)>::max()/2;//0x7FFFFFFF;
//
//
//void Twiddler::setRatio(double ratio){
//  if(ratio<=0){
//    freeze(false);
//  } else if(ratio >= 1.0) {
//    IntegerTwiddler::setRatio(MaxRes * ratio, MaxRes);
//  } else {
//    IntegerTwiddler::setRatio(MaxRes, MaxRes * ratio);
//  }
//}
//
//double Twiddler::getRatio(){
//  return ratio(double(above), double(below)); //use double so as to not lose accuracy on ints near to max.
//}
//
//double Twiddler::getDivider()const{
//  if(below==0){
//    return 0.0;//don't trust ratio(0,0) to stay the same over the ages :)
//  }
//  return ratio(double(below), double(above));
//}

/////////////////////
PwmModulator::PwmModulator(){
  quoticks = 0;
}

void PwmModulator::setRatio(double ratio){
  quoticks = unsigned(ratio); //#truncation desired
  setRatio(ratio - quoticks); //todo:3 find math function for getting fraction.
}

double PwmModulator::getRatio()const {
  return getRatio() + quoticks;
}

unsigned PwmModulator::pwm() const{
  return quoticks + pwm();
}

unsigned PwmModulator::quanta(unsigned chunk,bool noFloat){
  if(noFloat){
    //the below ignores the fraction part of dither, in order to not use floating point
    return ::quanta(chunk, quoticks);
  } else {
    return ratio(double(chunk), quoticks+ ratio(double(above), double(below)));
  }
}
