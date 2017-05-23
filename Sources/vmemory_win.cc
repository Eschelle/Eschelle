#include "common.h"
#if defined(OS_IS_WINDOWS)
#include <iostream>
#include <windows.h>
#include "vmemory.h"

namespace Eschelle{
    MemoryRegion::MemoryRegion(word size){
        void* addr = VirtualAlloc(nullptr, static_cast<size_t>(size), MEM_RESERVE, PAGE_NOACCESS);
        if(addr == nullptr){
            std::cerr << "Unable to virtual alloc: " << size << std::endl;
            getchar();
            abort();
        }
        ptr_ = addr;
        size_ = static_cast<uword>(size);
    }

    MemoryRegion::~MemoryRegion(){
        if(ptr_ != nullptr && size_ > 0){
            if(VirtualFree(ptr_, 0, MEM_RELEASE) == 0){
                std::cerr << "Unable to virtual free: " << ptr_ << "$" << size_ << std::endl;
                getchar();
                abort();
            }
        }
    }

    bool MemoryRegion::Protect(ProtectionMode mode){
        DWORD prot = 0;
        switch(mode){
            case kNoAccess: prot = PAGE_NOACCESS; break;
            case kReadOnly: prot = PAGE_READONLY; break;
            case kReadWrite: prot = PAGE_READWRITE; break;
            case kReadExecute: prot = PAGE_EXECUTE_READ; break;
            case kReadWriteExecute: prot = PAGE_EXECUTE_READWRITE; break;
        }
        DWORD old_prot = 0;
        return static_cast<bool>(VirtualProtect(ptr_, size_, prot, &old_prot));
    }
}

#endif // OS_IS_WINDOWS