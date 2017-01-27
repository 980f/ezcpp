#ifndef CIRCULARPOINTER_H
#define CIRCULARPOINTER_H

/** concrete base class, extracted from template for out-of-line efficiency */
class CircularPointerVoidly {
  /* this can be any class that takes up just one byte of memory*/
  typedef char Bytely;
protected:
  Bytely *pointer;
  Bytely *end; // actually one past the end
  unsigned step;
  Bytely *base;

  CircularPointerVoidly(void *base, unsigned step);

public:
  void rewind();

  void move(int amount);

  void increment();

  void decrement();
};

template <typename Scalar> class CircularPointer: public CircularPointerVoidly {
  unsigned length;

public:
  CircularPointer(Scalar *base, unsigned length): CircularPointerVoidly(base, sizeof(Scalar)),
    length(length){
    if(length == 0) {
      end = pointer = nullptr; // setting pointer null simpifies validity check
    } else if(base == nullptr) {
      end = nullptr;
    } else {
      end = &(base[length]);
    }
  }

  /** peek, preview of what @see next will return. NB: can return a null reference. */
  inline Scalar &operator *() const {
    return *reinterpret_cast<Scalar *>(pointer);
  }

  operator Scalar *() const {
    return reinterpret_cast<Scalar *>(pointer);
  }

  /** NB: can return a null reference if buffer is defective */
  Scalar &next(){
    Scalar &item = *reinterpret_cast<Scalar *>(pointer);
    increment();
    return item;
  }

  CircularPointer &operator +=(int amount){
    move( amount);
    return *this;
  } // +=

  CircularPointer &operator -=(int amount){
    move(-amount);
    return this;
  }

  /** pre increment */
  CircularPointer &operator ++(){
    increment();
    return this;
  }

  /** pre-decrement */
  CircularPointer &operator --(){
    decrement();
    return this;
  }

private:
  /** post-increment: can't find a use (nor cost-effective) use for this, see next() function.*/
  CircularPointer &operator ++(int) = delete;
  /** post-decrement: can't find a use (nor cost-effective) use for this, see next() function.*/
  CircularPointer &operator --(int) = delete;
};

#endif // CIRCULARPOINTER_H
