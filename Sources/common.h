#ifndef ESCHELLE_COMMON_H
#define ESCHELLE_COMMON_H

#include <cstdint>
#include <cstring>
#include <cstddef>

namespace Eschelle{
    typedef intptr_t word;
    typedef uintptr_t uword;

    static const size_t kWordSize = sizeof(word);

    const intptr_t kOffsetOfPtr = 32;

#define OFFSET_OF(Type, Field) \
    (reinterpret_cast<intptr_t>(&(reinterpret_cast<Type*>(kOffsetOfPtr)->Field)) - kOffsetOfPtr)

#if defined(_M_X64) || defined(__x86_64__)
#define ARCH_IS_X64 1
#define kFpuRegisterSize 16
#elif defined(_M_IX86) || defined(__i386__)
#define ARCH_IS_X32 1
#define kFpuRegisterSize 16
#elif defined(__ARMEL__)
#define ARCH_IS_ARM 1
#define kFpuRegisterSize 16
#elif defined(__aarch64__)
#define ARCH_IS_ARM64 1
#define kFpuRegisterSize 16
#endif

    static inline uword RoundUpPowTwo(uword x){
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

#ifndef ESCH_DEBUG
#define ESCH_DEBUG 1
#endif // ESCH_DEBUG

#if defined(__linux__) || defined(__FreeBSD__)
#define OS_IS_LINUX 1
#elif defined(__APPLE__)
#define OS_IS_OSX 1
#elif defined(_WIN32)
#define OS_IS_WINDOWS 1
#endif
}

#endif //ESCHELLE_COMMON_H