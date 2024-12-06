#pragma once
#define CONTINUEDFRACTIONRATIOGENERATOR_H  //(C) 2018-2019 Andy Heilveil, github/980f

/** find a ratio of integers which best matches a floating point number.
 * This is useful for tuning a PWM.
 *
 * It sometimes gives something that isn't the absolute best due to numerical error in the 1/double computation.
 * One can extend it with something that computes the estimate on each pass and records the best as the iteration seeks it. That will nearly double the computational burden so we won't do that in this class.
 * Another variation would be a class for trying to reduce a ratio of integers to fit into a smaller integer representation.
 *
 * Note: during development the ratio was easily inverted by minor changes to the initialization, which leans a tad illegitimately on the iterative step to save on some code.
 * The terse variable names match the wikipedia article this math was extracted from.
 */
class ContinuedFractionRatioGenerator {
public:
  double fraction;
  unsigned an;
  unsigned h[3];//recursion relationship refers to 0,-1,-2 indices.
  unsigned k[3];
  /** how large the numerator or denominator may be.
   * The minimal constraint is to not overflow the computer representation.
   * This is usually a power of 2.
   */
  unsigned limit;
public:
  /** run the algorithm for the best match to @param ratio where no number is greater than @param limit.
      if the limit is zero then the maximum for the unsigned type on the platform is used.
      @returns &this
  */
  ContinuedFractionRatioGenerator& run(double ratio,unsigned limit = 0){
    restart(ratio,limit);
    best();
    return *this;
  }

  /** create one for learning the algorithm*/
  ContinuedFractionRatioGenerator();

  /** @returns the best integer ratio for @param ratio. Relies upon copy constructor which is not cheap. */
  static ContinuedFractionRatioGenerator Run(double ratio,unsigned limit = 0){
    ContinuedFractionRatioGenerator generator;
    generator.restart(ratio,limit);
    generator.best();
    return generator;
  }

  /** numerator and denominator will be <limit when iteration stops. for hardware use value should be ((1<<numbits)-1)
   * @param ratio is expected to be positive, if not hilarity ensues.
   * @returns 'OK to iterate' which is not yet sane, ignore it.
   */
  bool restart(double ratio,unsigned limit = ~0U);

  /** @returns whether the number format allows a more precise computation, if so then computation has been done and stored internally.
    The runtime cost is (1) 1/double, (2) double to int, (1) double subtract,  (2) 64 bit multiply/adds as well as half a dozen assigns and function calls.
    As such this should not be called in ISR's :)
  */
  bool step();

  /** @returns a number greater than denominator if input value is greater than 1 */
  unsigned numerator() const {
    return k[1];
  }

  /** @returns a number greater than numerator if input value is less than 1 */
  unsigned denominator() const {
    return h[1];
  }

  /** @returns present value of approximation. */
  double approximation() const;

  /** calls steps() until it says to stop then @returns approximation()
    This is a time consuming call.
    */
  double best();

private:
  bool split();
  bool bump(unsigned hk[]);
}; // class ContinuedFractionRatioGenerator


