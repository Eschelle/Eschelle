#include <iostream>
#include "alloc.h"

namespace Eschelle{
    Heap* Allocator::eden_ = new Heap(ESCH_HEAP_SIZE);
    Heap* Allocator::survivor_ = new Heap(ESCH_HEAP_SIZE);
    Heap* Allocator::tenured_ = new Heap(ESCH_HEAP_SIZE);
    Heap* Allocator::large_ = new Heap(ESCH_LSO_SIZE);
    Array<Allocator::AllocPage*> Allocator::pages_ = Array<Allocator::AllocPage*>(10);

    void* Allocator::MinorAlloc(word size){
        if(size == 0) return nullptr;
        size += sizeof(AllocPage);
        if(size > ESCH_GC_MIN_SIZE){
            return Allocator::MajorAlloc(size - sizeof(AllocPage));
        }

        void* raw = eden_->Alloc(size);
        if(raw == nullptr){
            //TODO: Scavenge?
            return MinorAlloc(size - sizeof(AllocPage));
        }
        void* ptr = reinterpret_cast<char*>(raw) + sizeof(AllocPage);
        pages_.Add(new AllocPage(size - sizeof(AllocPage), ptr));
        return ptr;
    }

    void* Allocator::MajorAlloc(word size){
        //TODO: Implement
        return nullptr;
    }

    void* Allocator::Alloc(word size){
        void* ptr = MinorAlloc(size);
        if(ptr == nullptr) ptr = MajorAlloc(size);
        return ptr;
    }

#define CHUNK(ptr) (reinterpret_cast<char*>(ptr) - sizeof(size_t))
#define SIZE(ptr) (*(reinterpret_cast<size_t*>(ptr)))
#define POSITION (reinterpret_cast<uword>((curr)))

    void Heap::Visit(HeapVisitor* vis){
        void* curr = start_;
        while(POSITION < End()){
            vis->Visit(curr, SIZE(curr));
            curr = reinterpret_cast<char*>(curr) + SIZE(curr);
            if(SIZE(curr) == 0) break;
        }
    }

    class HeapPrinter : public HeapVisitor{
    private:
        word count_;

        friend class Allocator;
    public:
        HeapPrinter():
                count_(0){}
        ~HeapPrinter(){}

        void Visit(void* ptr, size_t size){
            count_++;
            std::cout << ptr << ": " << size << std::endl;
        }
    };

    void Allocator::PrintHeap(GenerationSpace space) {
        HeapPrinter printer;
        std::cout << "*** List of heap allocated chunks" << std::endl;
        std::cout << "*** Generation: ";
        switch(space){
            case kEden:{
                std::cout << "Eden" << std::endl;
                eden_->Visit(&printer);
                break;
            }
            case kSurvivor:{
                std::cout << "Survivor" << std::endl;
                survivor_->Visit(&printer);
                break;
            }
            case kTenured:{
                std::cout << "Tenured" << std::endl;
                tenured_->Visit(&printer);
                break;
            }
            case kLargeObject:{
                std::cout << "Large Object" << std::endl;
                large_->Visit(&printer);
                break;
            }
        }
        std::cout << "*** " << printer.count_ << " Chunks" << std::endl;
        std::cout << "*** End of heap chunks list" << std::endl;
    }
}