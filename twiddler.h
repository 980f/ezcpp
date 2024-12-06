#pragma once

#if !defined (EZ_NoFloatingPoint)
#define EZ_NoFloatingPoint true
#endif
/** if this is true ensure that no references are made to floating point math at all*/
const bool noFloat=EZ_NoFloatingPoint;

/** first use was software PWM. Origin is from Bresneham Line Draw, as refined by 980F
  * implements as a ratio of two integers without using division except during some of the optional setup functions.
  */

template<typename Integral> /*todo:1 constraint to signed */ class Interpolator {
protected:
  Integral above;
  Integral below;
  Integral twiddle;
public:
  Interpolator(){
    freeze();
  }
  /** @see setRatio */
  constexpr Interpolator(Integral numer, Integral denom, bool center = true);

  /** @param numer out of (@param numer + @param denom) calls (in a row) to pwm() will return true.
*  @param center true initializes the cycling to halfway through the longer part of the cycle.*/
  void setRatio(Integral numer, Integral denom, bool center = true){
    above = numer;
    below = denom;
    if(center) {
      twiddle = (above - below) / 2;
    } else {
      if(twiddle > below) {
        twiddle = below;
      } else if(twiddle < -above) {
        twiddle = -above;
      }
    }
    }

  /** set ratio such that pwm is true once per @param rate calls
    * NB: rate of 0 makes pwm 'always true'*/
  void setDivider(Integral rate) {if(rate<=0){
    freeze(false);
  } else {
    above = 1;
    below = rate;
    twiddle = -rate/2;
  }
  }
  /** rig for pwm() to always return @param on*/
  void freeze(bool on = false){
    above = 0;
    below = 0;
    twiddle = on - 1; //#hack
  }


  /** @returns a bit the average of which is the ratio of the above and below members */
  bool pwm(){
    if(twiddle >= 0) {
      twiddle -= below;
      return true;
    } else {
      twiddle += above;
      return false;
    }
  }

  operator bool (void){
    return pwm();
  }
};

///** add some convenient rate setting/getting functions, factored out of prior implementation to ge ta no-floating point base class */
//class Twiddler:public Interpolator<int> {
//public:
////  using IntegerTwiddler::IntegerTwiddler;
////  using IntegerTwiddler::setRatio;//this was supposed to give access to setRatio(unsigned,unsigned,..._) but that quit working.
//  Twiddler(double rate=0.0);
//  void setRatio(double ratio);
//  /**@return amount of time that twiddler is 'true' */
//  double getRatio(void) const;
//  /** @return 1/getRatio()*/
//  double getDivider(void) const;
//};

/** pulse width modulator modulator (not redundant!)
  * modulates a value intended for use in pwm signal to
  * average to a given value */
class PwmModulator : public Interpolator<int> {
  unsigned quoticks; //subtract or add this amount per tick for accel/decel respectively
public:
  PwmModulator();
  void setRatio(double ratio);
  double getRatio() const;
  unsigned pwm() const;
  /** this value will vary such that its average will approach the ratio.*/
  operator unsigned() {
    return pwm();
  }
  /** noFloat gives a rougher estimate but doesn't use floating point, relevent to an ISR*/
  unsigned quanta(unsigned chunk,bool noFloat=true);
};
