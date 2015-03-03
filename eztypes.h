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

//for when you have to do something sneaky:
#define pun(type, lvalue) (*reinterpret_cast <type *> (&(lvalue)))

/**for when you want to live dangerously.
 * We use the convention that all references have been confirmed non-null.
 * only use this when you are feeding an &(expression) */
#define NULLREFERENCE(type) (*reinterpret_cast <type *> (0))

//lord it would be nice if C would make a standard operator for this:
#define countof(array) (sizeof(array) / sizeof(array[0]))

///** for a private single instance block */
//#define soliton(type, address) type & the ## type = *reinterpret_cast <type *> (address);

//in case some other compiler is used someday, this is gcc specific:
#define PACKED __attribute__ ((packed))

#if isQCU || isPCU
//function is (possibly) used in an isr, should be speed optimized:
#define ISRISH __attribute__((optimize(3)))
#else
#define ISRISH
#endif


//note: the linker is the agent that makes this happen. It failed in the past with Rowley scripts as they left out SORT(...)
//requires proper linker script:
#define InitStep(k) __attribute__((init_priority(k)))
//lower happens first. some dox say leave 0..1024 for the compiler.
#define InitHardware 10000
#define InitUserLibrary 20000
#define InitApplication 30000

#if __linux__
#define NAKED
#else
//the following won't compile while the defined value is just fine ?!?
#define NAKED __attribute__((naked))
#endif

#if 0 //missing on windows (mingw) platform compiler, expected by cwchar
void swprintf();
void vswprintf();
#endif

#endif /* ifndef eztypes_h */
