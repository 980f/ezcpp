#ifndef BITBANGER_H
#define BITBANGER_H

/** bit and bitfield setting and getting.*/


/** @returns byte address argument as a pointer to that byte */
inline constexpr unsigned& atAddress(unsigned address){
  return *reinterpret_cast<unsigned *>(address);
}

constexpr bool bit(unsigned patter, unsigned bitnumber){
  return (patter & (1 << bitnumber)) != 0;
}


constexpr bool isOdd(unsigned pattern){
  return pattern&1;
}

constexpr bool isEven(unsigned pattern){
  return ! isOdd(pattern);
}

inline bool setBit(unsigned &patter, unsigned bitnumber){
  return patter |= (1 << bitnumber);
}

inline bool setBitAt(unsigned addr, unsigned bitnumber){
  return setBit(atAddress(addr),bitnumber);
}

inline bool clearBit(unsigned &patter, unsigned bitnumber){
  return patter &= ~(1 << bitnumber);
}

inline bool clearBitAt(unsigned addr, unsigned bitnumber){
  return clearBit(atAddress(addr),bitnumber);
}


/** ensure a 0:1 transition occurs on given bit. */
inline void raiseBit(unsigned &address, unsigned  bit){
  clearBit(address, bit);
  setBit(address, bit);
}

inline bool assignBit(unsigned &pattern, unsigned bitnumber,bool one){
  if(one){
    setBit(pattern,bitnumber);
  } else {
    clearBit(pattern,bitnumber);
  }
  return one;
}


/** use the following when only one of offset or width are constants */
constexpr unsigned int insertField(unsigned target, unsigned source, unsigned mask){
  return (target & ~mask) | (source & mask);
}

inline unsigned mergeInto(volatile unsigned &target, unsigned source, unsigned mask){
  return target= insertField(target,source, mask);
}


/** @returns bits @param msb through @param lsb set to 1.
 * Default arg allows one to pass a width for lsb aligned mask of that many bits */
constexpr unsigned fieldMask(unsigned msb,unsigned lsb=0){
  return (1 << (msb+1)) - (1<<lsb);
}

/** use the following when offset or width are NOT constants, else you should be able to define bit fields in a struct and let the compiler to any inserting*/
constexpr unsigned int insertField(unsigned target, unsigned source, unsigned msb, unsigned lsb){
  return insertField(target, source<<lsb ,fieldMask(msb,lsb));
}

inline unsigned mergeField(volatile unsigned &target, unsigned source, unsigned msb, unsigned lsb){
  return target=insertField(target,source,msb,lsb);
}

inline unsigned int extractField(unsigned int source, unsigned int msb, unsigned int lsb){
  return (source&fieldMask(msb,lsb)) >> lsb ;
}


/** @returns bits @param lsb for width @param width set to 1.
 * Default arg allows one to pass a width for lsb aligned mask of that many bits */
constexpr unsigned bitMask(unsigned lsb,unsigned width=1){
  return (1 << (lsb+width)) - (1<<lsb);
}

/** use the following when offset or width are NOT constants, else you should be able to define bit fields in a struct and let the compiler to any inserting*/
constexpr unsigned int insertBits(unsigned target, unsigned source, unsigned lsb, unsigned width){
  return insertField(target, source<<lsb ,bitMask(lsb,width));
}

inline unsigned mergeBits(volatile unsigned &target, unsigned source, unsigned lsb, unsigned width){
  return mergeInto(target,source<<lsb,bitMask(lsb,width));
}

constexpr unsigned extractBits(unsigned source, unsigned lsb, unsigned width){
  return (source & bitMask(lsb,width)) >> lsb ;
}



/** for when the bits to pick are referenced multiple times and are compile time constant
 * trying to bind the address as a template arg runs afould of the address not being knowable at compile time*/
template <unsigned lsb, unsigned msb, bool msbIsWidth=false> class BitFielder {
  enum {
    mask = msbIsWidth?bitMask(lsb,msb):fieldMask(msb ,lsb) // aligned mask
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
template <unsigned lsb, unsigned msb, bool msbIsWidth=false> class BitField: public BitFielder<lsb,msb,msbIsWidth> {
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
