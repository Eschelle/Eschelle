#ifndef ESCHELLE_GLOBALS_H
#define ESCHELLE_GLOBALS_H

#include <cstdint>
#include <cstddef>
#include <cstring>

#if defined(_M_X64) || defined(__x86_64__)
#define ARCH_IS_X64 1
#elif defined(_M_IX86) || defined(__i386__)
#define ARCH_IS_X32 1
#elif defined(__ARMEL__)
#define ARCH_IS_ARM 1
#elif defined(__aarch64__)
#define ARCH_IS_ARM64 1
#else
#error "Cannot determine cpu architecture"
#endif

#if defined(__linux__) || defined(__FreeBSD__)
#define OS_IS_LINUX 1
#elif defined(__APPLE__)
#define OS_IS_APPLE 1
#elif defined(_WIN32)
#define OS_IS_WINDOWS 1
#else
#error "Cannot determine os"
#endif

#if defined(_MSC)
#define ESCH_INLINE static __inline
#else
#define ESCH_INLINE static inline
#endif

namespace Eschelle{
  typedef uintptr_t uword;
  typedef intptr_t word;

  static const size_t kWordSize = sizeof(word);

  ESCH_INLINE uword
  RoundUpPow2(uword x){
    x = x - 1;
    x = x | (x >> 1);
    x = x | (x >> 2);
    x = x | (x >> 4);
    x = x | (x >> 8);
    x = x | (x >> 16);
#if defined(ARCH_IS_X64)
    x = x | (x >> 32);
#endif
    return x + 1;
  }
}

#endif //ESCHELLE_GLOBALS_H
