#include "common.h"
#if defined(OS_IS_OSX)
#include <sys/mman.h>
#include <iostream>
#include "vmemory.h"

#undef MAP_FAILED
#define MAP_FAILED reinterpret_cast<void*>(-1)

namespace Eschelle{
    MemoryRegion::~MemoryRegion(){
        if(ptr_ != nullptr && size_ > 0){
            if(munmap(ptr_, size_) != 0){
                std::cerr << "Failed to munmap memory region" << std::endl;
            }
        }
    }

    MemoryRegion::MemoryRegion(word size){
        void* addr = mmap(nullptr, size, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_NORESERVE, -1, 0);
        if(addr == MAP_FAILED){
            std::cerr << "Failed to mmap region" << std::endl;
            return;
        }
        ptr_ = addr;
        size_ = size;
    }

    bool MemoryRegion::Protect(ProtectionMode mode){
        int prot = 0;
        switch(mode){
            case kNoAccess: prot = PROT_NONE; break;
            case kReadOnly: prot = PROT_READ; break;
            case kReadWrite: prot = PROT_READ|PROT_WRITE; break;
            case kReadExecute: prot = PROT_READ|PROT_EXEC; break;
            case kReadWriteExecute: prot = PROT_READ|PROT_WRITE|PROT_EXEC; break;
        }
        return (mprotect(Pointer(), Size(), prot) == 0);
    }
}

#endif // OS_IS_OSX