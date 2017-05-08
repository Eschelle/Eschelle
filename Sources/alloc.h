#ifndef ESCHELLE_ALLOC_H
#define ESCHELLE_ALLOC_H

#include <cstring>
#include "common.h"
#include "array.h"

#ifndef ESCH_HEAP_SIZE
#define ESCH_HEAP_SIZE 1024
#endif // ESCH_HEAP_SIZE

#ifndef ESCH_LSO_SIZE
#define ESCH_LSO_SIZE 64 * 1024
#endif // ESCH_LSO_SIZE

#ifndef ESCH_GC_MIN_SIZE
#define ESCH_GC_MIN_SIZE 256
#endif // ESCH_GC_MIN_SIZE

namespace Eschelle{
    class HeapVisitor;

    class Heap{
    private:
        void* start_;
        void* current_;
        uword size_;

        void Visit(HeapVisitor* vis);

        friend class Allocator;
    public:
        Heap(word size){
            if(size > 0){
                size_ = RoundUpPowTwo(size);
                start_ = current_ = malloc(size_);
            }
        }
        ~Heap(){
            free(start_);
        }

        uword AllocatedSize() const{
            return reinterpret_cast<char*>(current_) -
                    reinterpret_cast<char*>(start_);
        }

        uword TotalSize() const{
            return size_;
        }

        uword Start() const{
            return reinterpret_cast<uword>(start_);
        }

        uword End() const{
            return Start() + TotalSize();
        }

        bool Contains(void* ptr) const{
            uword p = reinterpret_cast<uword>(ptr);
            return (p >= Start()) &&
                    (p <= End());
        }

        void* Alloc(size_t size){
            size += sizeof(size_t);
            void* chunk = current_;
            current_ = static_cast<char*>(current_) + size;
            memset(chunk, 0, size);
            (*(reinterpret_cast<size_t*>(chunk))) = size;
            chunk = static_cast<char*>(chunk) + sizeof(size_t);
            return chunk;
        }

        void Free(void* ptr){
            //TODO: Implement?
        }
    };

    class HeapVisitor{
    public:
        HeapVisitor(){}
        virtual ~HeapVisitor(){}

        virtual void Visit(void* ptr, size_t size) = 0;
    };

    enum GenerationSpace{
        kEden,
        kSurvivor,
        kTenured,
        kLargeObject
    };

    enum Color{
        kFree = 0,
        kWhite,
        kBlack,
        kGray
    };

    class Allocator{
    private:
        static Heap* eden_;
        static Heap* survivor_;
        static Heap* tenured_;
        static Heap* large_;

        static void* MinorAlloc(word size);
        static void* MajorAlloc(word size);

        struct AllocPage;

        struct ReferenceTable{
            AllocPage** table;
            size_t size;
            size_t asize;
        };

        struct AllocPage{
            ReferenceTable* refs;
            size_t size;
            Color color;
            bool marked : 1;
            void* ptr;

            AllocPage(size_t s, void* p):
                    refs(new ReferenceTable()),
                    size(s),
                    color(kFree),
                    ptr(p),
                    marked(false){
                refs->size = 0;
                refs->asize = 10;
                refs->table = reinterpret_cast<AllocPage**>(malloc(sizeof(AllocPage*) * 10));
            }
        };

        static Array<AllocPage*> pages_;
    public:
        static void* Alloc(word size);

        static size_t PageSize(){
            return sizeof(AllocPage) + sizeof(size_t);
        }

#if ESCH_DEBUG
        static void PrintHeap(GenerationSpace space);
#endif
    };
}

#endif //ESCHELLE_ALLOC_H
