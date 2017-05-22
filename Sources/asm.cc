#include "asm.h"
#include <cstdlib>

namespace Eschelle{
    static const word kMinGap = 32;
    static const word kInitialBufferCapacity = 4 * (1024 * 1024);

    static inline uword AllocSafe(word size){
        return reinterpret_cast<uword>(malloc(size));
    }

    static inline uword ComputeLimit(uword data, word cap){
        return data + cap + kMinGap;
    }

    AssemblerBuffer::AssemblerBuffer(){
        contents_ = cursor_ = AllocSafe(kInitialBufferCapacity);
        limit_ = ComputeLimit(contents_, kInitialBufferCapacity);
    }

    void AssemblerBuffer::Commit(MemoryRegion* region){
        region->Protect(MemoryRegion::kReadWriteExecute);
        region->CopyFrom(Contents(), Size());
        region->Protect(MemoryRegion::kReadExecute);
    }
}