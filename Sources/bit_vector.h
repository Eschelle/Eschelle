#ifndef ESCHELLE_BIT_VECTOR_H
#define ESCHELLE_BIT_VECTOR_H

#include "common.h"

namespace Eschelle{
    class BitVector{
    private:
        word size_;
        word asize_;
        uword* data_;
    public:
        static const int kBitsPerByte = 8;
        static const int kBitsPerWord = sizeof(word) * 8;

        BitVector(word size):
                size_(size),
                asize_(1 + ((size - 1) / kBitsPerWord)){
            data_ = reinterpret_cast<uword*>(malloc(sizeof(uword) * asize_));
            Clear();
        }

        void Clear(){
            for(int i = 0; i < asize_; i++) data_[i] = 0x0;
        }

        void Add(word i){
            data_[i / kBitsPerWord] |= static_cast<uword>(1 << (i % kBitsPerWord));
        }

        void Remove(word i){
            data_[i / kBitsPerWord] &= ~static_cast<uword>(1 << (i % kBitsPerWord));
        }

        void Intersect(BitVector* other){
            for(int i = 0; i < asize_; i++) data_[i] = data_[i] & other->data_[i];
        }

        bool Contains(word i){
            uword block = data_[i / kBitsPerWord];
            return (block & static_cast<uword>(1 << (i % kBitsPerWord))) != 0;
        }

        bool AddAll(BitVector* from){
            bool changed = false;

            for(int i = 0; i < asize_; i++){
                uword before = data_[i];
                uword after = data_[i] | from->data_[i];
                if(before != after){
                    changed = true;
                    data_[i] = after;
                }
            }

            return changed;
        }

        bool KillAndAdd(BitVector* kill, BitVector* gen){
            bool changed = false;

            for(int i = 0; i < asize_; i++){
                uword before = data_[i];
                uword after = data_[i] | (gen->data_[i] & ~kill->data_[i]);
                if(before != after){
                    changed = true;
                    data_[i] = after;
                }
            }

            return changed;
        }
    };
}

#endif //ESCHELLE_BIT_VECTOR_H