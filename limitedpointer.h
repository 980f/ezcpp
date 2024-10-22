#ifndef LIMITEDPOINTER_H
#define LIMITEDPOINTER_H
#include <stdint.h>

/** a pointer into an externally allocated array of some type. Given that range this keeps itself within that range (if you let it).
  The class is slightly inconsistent in that if you run off the end you get the beginning from then on (if there is one), 
  although under many conditions you will get slapped with a SEGV/NPE if you have ignored the validity checking access methods.
  
  This allows an embedded program to keep on running with a defect that you will hopefully notice rather than going tits up in code that might not matter much.
*/

template <typename Scalar> class LimitedPointer {
    Scalar * const base;
    Scalar * const end; // actually one past the end
    Scalar *pointer;
    
  public:
  /** using a signed quantity so that an all-ones marker value for 'none' works the same as 0 */
    LimitedPointer(Scalar *theBase, size_t quantity = 0):
      base( (quantity > 0) ? theBase : nullptr),
      end( (quantity > 0) ? &(base[quantity]) : nullptr { 
      pointer = base; 
    }

    LimitedPointer(): LimitedPointer(nullptr) {
      //this is an invalid, not wellDefined instance. 
    }

    /** @returns whether next() will return an expected value, i.e. whether it makes sense to call next(). 
      This is a weaker operation than the operator bool() version, for urgent loops where wellDefined() can be checked once. */
    inline bool isValid()const {
      return pointer < end;
    }

    /** @returns whether there is at least one element in the buffer.
      At one time returned true even if the buffer was empty (zero content allocated, but address was valid)
     */
    inline bool wellDefined() const {
      return base != nullptr && end > base;
    }

    /** @returns whether @see wellDefined() and @see isValid() */
    inline operator bool() const {
      return wellDefined() && isValid();
    }

    /** use like pointer. If the object is not also wellDefined then use of this guy might get you an NPE */
    inline Scalar &operator *() const {
      return isValid() ? *pointer : *base;
    }

    /** @returns the recent selection, or the 1st, which might be nullptr */
    operator Scalar *() const {
      if (isValid()) {
        return pointer;
      } else {
        return base;
      }
    }
    
    /** @returns object offset by @param index relative to this pointer, but base object if index is out of range. */
    Scalar &operator [](int index) const {
      Scalar *proposed = pointer += index;
      if (proposed >= end) {
        return *base;
      } else if (proposed < base) {
        return *base;
      } else {
        return *proposed;
      }
    }

    Scalar &next() {
      if (this->operator bool) { 
        return *pointer++;
      } else {
        return *base; // on defective access point to the entity most likely to be noticed if trashed.
      }
    }

    /** moves pointer given amount, limiting to one past the end or to the first if out of range. */
    LimitedPointer &operator +=(int amount) {
      Scalar *proposed = pointer + amount;

      if (proposed >= end) {
        pointer = end; //value here and below are chosen for easy debug of most common fault cases.
      } else if (proposed < base) {
        pointer = base;
      } else {
        pointer = proposed;
      }
      return *this;
    }

    LimitedPointer &operator -=(int amount) {
      return this->operator +=(-amount);
    }

    /** pre increment */
    LimitedPointer &operator ++() {
      return this->operator +=(1);
    }

    /** pre-decrement */
    LimitedPointer &operator --() {
      return this->operator +=(-1);
    }

  private:
    /** post-increment: can't find a use (nor cost-effective) use for this, see next() function.*/
    LimitedPointer &operator ++(int) {
      return this->operator +=(1);
    }
    /** post-decrement: can't find a use (nor cost-effective) use for this, see next() function.*/
    LimitedPointer &operator --(int) {
      return this->operator -=(1);
    }

  public:
    /** set back to first */
    LimitedPointer &rewind() {
      pointer = base;
      return *this;
    }
    /** @return pointer to an entity in the defined range, or nullptr.
       NB: Ignores current position of 'this', giving read access to the internal buffer pointer. @see operator [] for relative neighbor.*/
    Scalar *neighbor(unsigned index)const {
      if (!wellDefined()) {
        return nullptr;
      }
      Scalar *proposed = base + index;
      if (proposed >= end) {
        return nullptr;
      } else if (proposed < base) {
        return nullptr;
      } else {
        return proposed;
      }
    }
};

#endif // LIMITEDPOINTER_H
