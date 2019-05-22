#ifndef LIMITEDPOINTER_H
#define LIMITEDPOINTER_H
#include <stdint.h>

/** a pointer into an externally allocated array of some type. Given that range this keeps itself within that range (if you let it) */

template <typename Scalar> class LimitedPointer {
    Scalar *pointer;
    Scalar *base;
    Scalar *end; // actually one past the end

  public:
    LimitedPointer(Scalar *base, unsigned quantity):
      base(base) {
      pointer = base;
      if (base == nullptr) {
        end = nullptr;
      } else {
        end = &(base[quantity]);
      }
    }

    LimitedPointer(): LimitedPointer(nullptr, 0) {

    }

    /** @returns whether next() will return an expected value, i.e. whether it makes sense to call next(). This is a weaker operation than the operator bool() version, for urgent loops where wellDefined() can be checked once. */
    inline bool isValid()const {
      return pointer < end;
    }

    inline bool wellDefined() const {
      return base != nullptr && end >= base;
    }

    /** @returns whether @see wellDefined() and @see isValid() */
    inline operator bool() const {
      return wellDefined() && isValid();
    }

    /** use like pointer */
    inline Scalar &operator *() const {
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
    /** @returns object offset by @param index relative to this pointer, but base object if index is out of range. */
    Scalar &operator [](int index) const {
      Scalar *proposed = pointer += index;
      if (proposed >= end) {
        return *base;
      } else if (proposed < base) {
        return *base;
      } else {
        return* proposed;
      }
    }

    Scalar &next() {
      if (*this) {
        return *pointer++;
      } else {
        return *base; // on defective access point to the entity most likely to be noticed if trashed.
      }
    }

    /** moves pointer given amount, limiting to one past the end or to the first if out of range. */
    LimitedPointer &operator +=(int amount) {
      Scalar *proposed = pointer + amount;

      if (proposed >= end) {
        pointer = end;
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
