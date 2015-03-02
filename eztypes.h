#ifndef eztypes_h
#define eztypes_h

// platform differences manager
#include <stdint.h>  // <cstdint> not available on micro.

// underscores are physiologically painful to type:
// typedef uint8_t u8;
typedef unsigned char u8;
// old std lib stuff wants plain char *'s, grrr:
#define U8S(plaincharstar) (reinterpret_cast<const u8 *>(plaincharstar))
#define U8Z(u8star) (reinterpret_cast<const char *>(u8star))

typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

/** used to assign bit pattern from one type to another, code that uses this is not portable! */
template <typename Result,typename Other> constexpr Result &punt(Other &other){
  return *reinterpret_cast<Result *>(&(other));
}

// lord it would be nice if C would make a standard operator for this:
#define countof(array) (sizeof(array) / sizeof(array[0]))


/** for a private single instance block */
#define soliton(type, address) type * const the ## type = reinterpret_cast<type *>(address);

/** instantiate one of these as a local variable at the start of a compound statement to ensure the given 'lock' bit is set to !polarity for all exit paths of that block
 */
class BitLock {
  u32 &locker; // stm32 bit band address of something
  u32 polarity;
/** construction assigns to the lock bit*/
  BitLock( u32 &lockBit, u32 _polarity): locker(lockBit), polarity(_polarity){
    locker = polarity;
  }
  /** destruction assigns to opposite of original assignement, blowing away any interim assignments */
  ~BitLock(){
    locker = ! polarity;
  }
};

// in case some other compiler is used someday, this is gcc specific:
#define PACKED __attribute__ ((packed))

#if HOST_SIM
//not writing isr's, compile the code as a normal function.
#define ISRISH
#else
// function is used in an isr, should be speed optimized:
#define ISRISH __attribute__((optimize(3)))
#endif

// never worked per spec, not sure if it was perfectly wrong: #define InitStep(k) __attribute__((init_priority(k)))

#if 0 // missing on windows (mingw) platform compiler, expected by cwchar
void swprintf();
void vswprintf();
#endif



#endif /* ifndef eztypes_h */
