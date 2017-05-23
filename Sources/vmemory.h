#ifndef ESCHELLE_MEM_REGION_H
#define ESCHELLE_MEM_REGION_H

#include "common.h"
#include <cstring>

namespace Eschelle{
    class MemoryRegion{
    private:
        void* ptr_;
        uword size_;
    public:
        enum ProtectionMode{
            kNoAccess,
            kReadOnly,
            kReadWrite,
            kReadExecute,
            kReadWriteExecute
        };
        MemoryRegion():
                size_(0),
                ptr_(nullptr){}
        MemoryRegion(word size);
        MemoryRegion(void* ptr, uword size):
                size_(size),
                ptr_(ptr){}
        MemoryRegion& operator=(const MemoryRegion& other){
            ptr_ = other.ptr_;
            size_ = other.size_;
            return *this;
        }
        ~MemoryRegion();

        void* Pointer() const{
            return ptr_;
        }

        uword Size() const{
            return size_;
        }

        uword Start() const{
            return reinterpret_cast<uword>(Pointer());
        }

        uword End() const{
            return Start() + size_;
        }

        void CopyFrom(const MemoryRegion& other) const{
            memcpy(reinterpret_cast<void*>(Start()), other.Pointer(), other.Size());
        }

        void CopyFrom(void* ptr, word size){
            memcpy(ptr_, ptr, size);
        }

        bool Protect(ProtectionMode mode);
    };
}

#endif //ESCHELLE_MEM_REGION_H