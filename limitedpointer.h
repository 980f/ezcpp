#ifndef LIMITEDPOINTER_H
#define LIMITEDPOINTER_H
#include <stdint.h>

/** a pointer into an externally allocated array of some type. Given that range this keeps itself within that range (if you let it).
  The class is slightly inconsistent in that if you run off the end you get the beginning from then on (if there is one),
  although under many conditions you will get slapped with a SEGV/NPE if you have ignored the validity checking access methods.

  This allows an embedded program to keep on running with a defect that you will hopefully notice rather than going tits up in code that might not matter much.
*/

template <typename Scalar> class LimitedPointer {
    Scalar *pointer;
    Scalar *base;
    Scalar *end; // actually one past the end, halfopen interval

  public:
  /** using a signed quantity so that an all-ones marker value for 'none' works the same as 0 */
    LimitedPointer(Scalar *theBase, size_t quantity = 0):
      base( (quantity > 0) ? theBase : nullptr),
      end( base&&(quantity > 0) ? &(base[quantity]) : nullptr {
      pointer = base;
    }

    /** @returns whether next() will (most likely) return an actual value, i.e. whether it makes sense to call next(). This is a weaker operation than the operator bool() version, for urgent loops where wellDefined() can be checked once. */
    bool isValid() const {
      return pointer < end;
    }

    /** @returns whether the block pointed to has at least one element */
    bool wellDefined() const {
      return base && end > base;
    }

    bool hasNext() const {
      return wellDefined() && isValid();
    }

    /** @returns a reference to an element of the block, IFFI the block is valid which is NOT checked.
 * This is only safe to use if you hae already tested wellDefined() && isValid(), which is what
     */
    Scalar &next() {
      if (*this) {
        return *pointer++;
      } else {
        return *base; // on defective access point to the entity most likely to be noticed if trashed.
      }
    }

    /** @returns whether @see wellDefined() and @see isValid() */
    operator bool() const {
      return hasNext();
    }

    /** use like pointer. If object is invalid expect an NPE in caller code */
    Scalar &operator *() const {
      return isValid() ? *pointer : *base;
    }

    /** @returns the recent selection, or the 1st, which might be nullptr */
    operator Scalar *() const {
      if (isValid()) {
        return pointer;
      } else {
        return base; // on defective access point to the entity most likely to be noticed if trashed.
      }
    }

    /** @returns object offset by @param index relative to this pointer, but base object if index is out of range.
 * not using unsigned as index -1 is 'previous' and that is a valid concept for this guy */
    Scalar &operator [](int index) const {
      Scalar &proposed = pointer[index];
      if (&proposed >= end) {
        return *base;
      } else if (&proposed < base) {
        return *base;
      } else {
        return proposed;
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
    /** @returns pointer to an entity in the defined range, or nullptr.
       NB: Ignores current position of 'this', giving read access to the internal buffer pointer. @see operator [] for relative neighbor.*/
    Scalar *neighbor(unsigned index) const {
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
