#ifndef ESCHELLE_ASM_H
#define ESCHELLE_ASM_H

#include "common.h"
#include "vmemory.h"

namespace Eschelle{
    class AssemblerBuffer{
    private:
        uword contents_;
        uword cursor_;
        uword limit_;
    public:
        AssemblerBuffer();

        template<typename T>
        void Emit(T value) {
            *(reinterpret_cast<T*>(cursor_)) = value;
            cursor_ += sizeof(T);
        }

        template<typename T>
        void Store(word pos, T value) {
            *(reinterpret_cast<T*>(contents_ + pos)) = value;
        }

        template<typename T>
        T Load(word pos){
            return reinterpret_cast<T>(contents_ + pos);
        }

        word Size() const{
            return cursor_ - contents_;
        }

        void* Contents() const{
            return reinterpret_cast<void*>(contents_);
        }

        void Commit(MemoryRegion* region);
    };
}

#if defined(ARCH_IS_X64)
#include "x64/asm.h"
#elif defined(ARCH_IS_X32)
#include "x32/asm.h"
#elif defined(ARCH_IS_ARM)
#include "arm/asm.h"
#elif defined(ARCH_IS_ARM64)
#include "arm64/asm.h"
#endif

#endif //ESCHELLE_ASM_H