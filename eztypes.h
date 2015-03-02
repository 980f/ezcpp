#ifndef eztypes_h
#define eztypes_h

//platform differences manager
#include <stdint.h>  //<cstdint> not available on micro.

// underscores are physiologically painful to type for some of us:
typedef uint8_t u8;
//old std lib stuff wants plain char *'s, grrr:
#define U8S(plaincharstar) (reinterpret_cast <const u8 *> (plaincharstar))
#define U8Z(u8star) (reinterpret_cast <const char *> (u8star))

typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

//legacy macro, don't use this for new code.
#define pun(type, lvalue) (*reinterpret_cast <type *> (&(lvalue)))

//lord it would be nice if C would make a standard operator for this:
#define countof(array) (sizeof(array) / sizeof(array[0]))

//for non-bit addressable items:
inline bool bit(int patter, unsigned bitnumber){
  return (patter & (1 << bitnumber)) != 0;
}

/** @returns @param width number of ls bits set to 1 */
inline unsigned int fieldMask(int width){
  return (1 << width) - 1;
}

/** use the following when offset or width are NOT constants, else you should be able to define bit fields in a struct and let the compiler to any inserting*/
inline unsigned int insertField(unsigned int target, unsigned int source, unsigned int offset, unsigned int width){
  unsigned int mask = fieldMask(width)<<offset;
  return (target & ~mask) | ((source << offset) & mask);
}

inline unsigned int extractField(unsigned int source, unsigned int offset, unsigned int width){
  return (source >> offset) & fieldMask(width);
}

/** for a private single instance block */
#define soliton(type, address) type & the ## type = *reinterpret_cast <type *> (address);

/** instantiate one of these as a local variable at the start of a compound statement to ensure the given 'lock' bit is set to !polarity for all exit paths of that block
  */
class BitLock {
  u32&locker; //for stm32 mcu this can be the bit band address of something
  u32 polarity;
/** construction assigns to the lock bit*/
  BitLock( u32 & lockBit, u32 _polarity): locker(lockBit), polarity(_polarity){
    locker = polarity;
  }
  /** destruction assigns to opposite of original assignment, blowing away any interim assignments */
  ~BitLock (){
    locker = !polarity;
  }
};

//in case some other compiler is used someday, this is gcc specific:
#define PACKED __attribute__ ((packed))

#if isQCU || isPCU
//function is (possibly) used in an isr, should be speed optimized:
#define ISRISH __attribute__((optimize(3)))
#else
#define ISRISH
#endif

//never worked per spec, not sure if it was perfectly wrong: #define InitStep(k) __attribute__((init_priority(k)))

#if 0 //missing on windows (mingw) platform compiler, expected by cwchar
void swprintf();
void vswprintf();
#endif

#endif /* ifndef eztypes_h */
