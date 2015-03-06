#ifndef BOOLISH_H
#define BOOLISH_H

/** allow on/off things to be passed around */
struct Boolish {
  /** @returns argument after setting the value */
  virtual bool operator =(bool)=0;
  virtual operator bool()=0;
};

/** for when the read can't afford to regenerate what was last written */
class CachedBoolish: public Boolish {
  mutable bool bit;
public:
  /** @see Boolish::operator= */
  virtual bool operator =(bool on);
  virtual operator bool();
};

/** on/off thing which is not changed while it alters some other boolean thing */
struct BoolishRef {
  /** @returns argument after setting the value */
  virtual bool operator =(bool)const=0;
  virtual operator bool()const=0;
};

#endif // BOOLISH_H
