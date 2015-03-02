#ifndef LIMITEDCOUNTER_H
#define LIMITEDCOUNTER_H

/** an unsigned integer that is coerced to stay between 0 and a construction-time limit value.
 *  There are two clone-like operations which can change that limit value, for reuse of objects, prefer the copy constructors when the expense of another object is tolerable.
 * The internal count value includes the limit value while many of the logic operations exclude it. This class is intended to be used as an iterator over a fixed-length array-like entity.
 */
class LimitedCounter {
protected:
  unsigned count;
  unsigned limit;
public:
  LimitedCounter(unsigned limit, int count = 0 ): count(count), limit(limit){
    if(count < 0) { // convenience to set count to limit
      this->count = limit;
    }
    if(this->limit < this->count) { // was a bug fix, might be nice to chase down usages and eliminate at source
      this->limit = this->count; // normalize error due to bad clip value in some use cases of Indexer.
    }
  }

  /** redefine this to be a limit count of present value of @param other. */
  void subset(const LimitedCounter &other){
    limit = other.count;
    count = 0;
  }

  /** become fully like @param other, versus operator= which only copies count value */
  void clone(const LimitedCounter &other){
    limit = other.limit;
    count = other.count;
  }

  /** copy constructor mostly used for creating a local entity from a passed argument.
   *  @param subset determines whether this one is the @see subset() of @param other or a @see clone() */
  LimitedCounter(const LimitedCounter &other, bool used = false){
    if(used) {
      subset(other);
    } else {
      clone(other);
    }
  }

  operator unsigned() const {
    return count;
  }

  /** and so many old-timey functions want an int */
  operator int(void) const {
    return count;
  }


  /**  @return whether index might be valid someday */
  bool canBe(unsigned index) const {
    return index < limit;
  }

  /** whether count is in its legal range, which excludes the limit value*/
  operator bool() const {
    return count < limit;
  }
public: // some names to mimic andyh's iteration preferences
  /** @returns whether count is not past the last */
  bool hasNext(void){
    return limit > 0 && count < limit;
  }

  /** @returns present count value then increments it. If this is not valid (@see operator bool()) then you get 0 */
  unsigned next(){
    if(*this) {
      return count++;
    } else {
      return 0;
    }
  }

  /** available for write, or if just snapped available for parsing*/
  int unused(void) const {
    return limit - count;
  }

  /** available for block write*/
  bool stillHas(int howmany) const {
    return unused() >= howmany;
  }
public: // some special assignments
  void clear(void){
    count = 0;
  }

  /** ensure subsequent hasNext() reports 'no' */
  void conclude(){
    count = limit; // our normal one-past-end state, to make sure multiple skips don't wrap.
  }

  /**
   * subtract @param backup from count, stopping at 0 if excessive.
   * @see rewind(void) for complete rewind rather than using a hopefully big enough value.
   * @return this
   */
  LimitedCounter &operator =(unsigned int jump){
    if(jump <= count) {
      count = jump;
    } else {
      count = limit; // canonical 'invalid' state
    }
    return *this;
  }

  /**
   * subtract @param backup from count, stopping at 0 if excessive.
   * @see rewind(void) for complete rewind rather than using a hopefully big enough value.
   * @return this
   */
  LimitedCounter &operator -=(unsigned int backup){
    if(backup <= count) {
      count -= backup;
    } else {
      count = 0; // rather than canonical 'invalid' state just start over.
    }
    return *this;
  }

  /**
   * subtract @param backup from count, stopping at 0 if excessive.
   * @see rewind(void) for complete rewind rather than using a hopefully big enough value.
   * @return this
   */
  LimitedCounter &operator +=(unsigned int amount){
    count += amount;
    if(count > limit) {
      count = limit; // our normal one-past-end state, to make sure multiple skips don't wrap.
    }
    return *this; // canonical 'invalid' state
  }
};

#endif // LIMITEDCOUNTER_H
