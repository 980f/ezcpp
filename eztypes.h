#pragma once

//platform differences manager
#include <stdint.h>  //<cstdint> not available on micro.

// underscores are physiologically painful to type for some of us:
typedef uint8_t u8;
//old std lib stuff wants plain char *'s, grrr:
#define U8S(plaincharstar) (reinterpret_cast <const u8 *> (plaincharstar))
#define U8Z(u8star) (reinterpret_cast <const char *> (u8star))

#ifndef __USBAPI__  //duplicate def with arduino USB libs
typedef uint16_t u16;
#endif
//the leonardo usbapi.h defines this as well as u16 while sam version only defines u16. <sigh/>
#if !(defined(__USBAPI__) && defined(ARDUINO_ARCH_AVR))
typedef uint32_t u32;
#endif

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

//in case some other compiler is used someday, this is gcc specific:
#define PACKED __attribute__ ((packed))

#if isQCU || isPCU
//function is (possibly) used in an isr, should be speed optimized:
#define ISRISH __attribute__((optimize(3)))
#else
#define ISRISH
#endif

//a function suitable for handling interrupts:
typedef void (*Handler)(void);

//note: the linker is the agent that makes this happen. It fails if your linker script leaves out SORT(...)
// creates section chunks like: .text._GLOBAL__sub_I.30000_startup and more importantly .init_array.30000 to call the previous which didn't actually need to be marked.
//requires proper linker script:
#define InitStep(k) __attribute__((init_priority(k)))
//lower happens first. some dox say leave 0..101 for the compiler.
#define InitHardware 10000
#define InitUserLibrary 20000
#define InitApplication 30000

#ifndef WEAK
#define WEAK __attribute__((weak))
#endif

#if __linux__
#define NAKED
#else
//the following won't compile while the defined value is just fine ?!?
#define NAKED __attribute__((naked))
#endif
