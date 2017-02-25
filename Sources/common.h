#ifndef ESCHELLE_COMMON_H
#define ESCHELLE_COMMON_H

#include <stdint.h>

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
}

#endif //ESCHELLE_COMMON_H