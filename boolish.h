#ifndef BOOLISH_H
#define BOOLISH_H

/** allow on/off things to be passed around */
struct Boolish {
  //do NOT add a virtual destructor, it casues linker headaches.
  //The cost of not being able to delete one of these without getting to its concrete class is worth this limitation.
  /** @returns argument after setting the value */
  virtual bool operator =(bool)=0;
  virtual operator bool()const=0;
   virtual void toggle(){
    this->operator=(1-bool(*this));
  }
};

/** on/off thing which is not changed while it alters some other boolean thing */
struct BoolishRef {
  //do NOT add a virtual destructor, it causes linker headaches.
  //The cost of not being able to delete one of these without getting to its concrete class is worth this limitation.
  /** @returns argument after setting the value */
  virtual bool operator =(bool)const=0;
  virtual operator bool()const=0;
  virtual void toggle()const{
    this->operator=(1-bool(*this));
  }
};

/** a const-able reference to a non-const bool. */
class BoolRef: public BoolishRef {
  bool &ref;
public:
  BoolRef(bool &ref):ref(ref){}
  bool operator =(bool value)const override{
    return ref=value;
  }
  operator bool()const override {
    return ref;
  }
};

/** for when the read can't afford to regenerate what was last written */
class CachedBoolish: public Boolish {
  mutable bool bit;
public:
  //do NOT add a virtual destructor, it casues linker headaches.
  //The cost of not being able to delete one of these without getting to its concrete class is worth this limitation.
  /** @see Boolish::operator= */
  virtual bool operator =(bool on);
  virtual operator bool()const;
};

/** creating one of these set the bit (to @param polarity) deleting it (typically automatic at end of scope) clears the bit. */
template <const BoolishRef &lockbit,bool polarity=1> struct LockBit {

  LockBit(){
    lockbit=polarity;
  }

  ~LockBit(){
    lockbit=!polarity;;
  }
};


#endif // BOOLISH_H
