#ifndef BITBANGER_H
#define BITBANGER_H

/** bit and bitfield setting and getting.*/

inline bool bit(int patter, unsigned bitnumber){
  return (patter & (1 << bitnumber)) != 0;
}

/** @returns bits @param msb through @param lsb set to 1.
 * Default arg allows one to pass a width for lsb aligned mask of that many bits */
constexpr unsigned fieldMask(unsigned msb,unsigned lsb=0){
  return (1 << (msb+1)) - (1<<lsb);
}

/** use the following when offset or width are NOT constants, else you should be able to define bit fields in a struct and let the compiler to any inserting*/
inline unsigned int insertField(unsigned target, unsigned source, unsigned msb, unsigned lsb){
  register unsigned mask = fieldMask(msb,lsb);
  return (target & ~mask) | ((source << lsb) & mask);
}

inline unsigned int extractField(unsigned int source, unsigned int msb, unsigned int lsb){
  return (source&fieldMask(msb,lsb)) >> lsb ;
}

/** for when the bits to pick are referenced multiple times
 * trying to bind the address as a template arg runs afould of the address not being knowable at compile time*/
template <unsigned lsb, unsigned msb> class BitFielder {
  enum {
    mask = fieldMask(msb ,lsb) // aligned mask
  };
public:
  unsigned extract(unsigned &item) const {
    return (item & mask) >> lsb;
  }

  unsigned mergeInto(unsigned &item,unsigned value) const {
    register unsigned merged= (item & ~mask) | ((value << lsb) & mask);
    item=merged;
    return merged;
  }
};


/** Create this object around a field of an actual data item.
 * trying to bind the address as a template arg runs afould of the address not being knowable at compile time*/
template <unsigned lsb, unsigned msb> class BitField: public BitFielder<lsb,msb> {
  unsigned &item;
public:
  BitField(unsigned &item): item(item){
  }
  operator unsigned() const {
    return BitFielder<lsb,msb>::extract(item);
  }
  void operator =(unsigned value) const {
    BitFielder<lsb,msb>mergeInto(item ,value );
  }
};



///////////////////////////////////////////
/// discontiguous bits, used for bitmmasking
///

/** declarative part of 3 step template magic */
template <unsigned ... list> struct BitWad;

/** termination case of 3 step template magic */
template <unsigned pos> struct BitWad<pos> {
  enum { mask = 1 << pos };
public:

  static bool exactly(unsigned varble, unsigned match){
    return (mask & varble) == (mask & match); // added mask to second term to allow for lazy programming
  }

  static bool all(unsigned varble){
    return (mask & varble) == mask;
  }

  static bool any(unsigned varble){
    return mask & varble;
  }

  static bool none(unsigned varble){
    return (mask & varble) == 0;
  }
};

/** assemble a bit field, without using stl. */
template <unsigned pos, unsigned ... poss> struct BitWad<pos, poss ...> {
  enum { mask = (1 << pos) | BitWad<poss ...>::mask };

public:
  static bool exactly(unsigned varble, unsigned match){
    return (mask & varble) == (mask & match); // added mask to second term to allow for lazy programming
  }

  static bool all(unsigned varble){
    return (mask & varble) == mask;
  }

  static bool any(unsigned varble){
    return mask & varble;
  }

  static bool none(unsigned varble){
    return (mask & varble) == 0;
  }
};

///trying to compact BitWad:
// template <unsigned mask> struct BitMasker {

// public:
//  static bool exactly(unsigned varble,unsigned match){
//    return (mask & varble) == (mask&match);//added mask to second term to allow for lazy programming
//  }

//  static bool all(unsigned varble){
//    return (mask & varble) == mask;
//  }

//  static bool any(unsigned varble){
//    return mask & varble;
//  }

//  static bool none(unsigned varble){
//    return (mask & varble) == 0;
//  }

// };


// template <unsigned ... list> constexpr unsigned maskBuilder();

// template <unsigned pos> constexpr unsigned maskBuilder<pos>(){
//  return 1<<pos;
// }

// template <unsigned pos,unsigned... poss> constexpr unsigned maskBuilder<pos,poss>{
//  return maskBuilder<pos>() | maskBuilder<poss...>();
// }

#endif // BITBANGER_H
