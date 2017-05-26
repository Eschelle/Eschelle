#ifndef ESCHELLE_ARRAY_H
#define ESCHELLE_ARRAY_H

#include <cstdlib>
#include "common.h"

namespace Eschelle{
    template<typename T>
    class Array{
    public:
        Array(word initialCap):
                length_(0),
                capacity_(0),
                data_(nullptr){
            if(initialCap > 0){
                capacity_ = RoundUpPowTwo(initialCap);
                data_ = reinterpret_cast<T*>(malloc(capacity_ * sizeof(T)));
            }
        }
        ~Array(){
            free(data_);
        }

        word Length() const{
            return length_;
        }

        T& operator[](word index) const{
            return data_[index];
        }

        T& Last() const{
            return operator[](length_ - 1);
        }

        T& Pop() {
            T& result = Last();
            length_--;
            return result;
        }

        void Add(const T& value){
            Resize(Length() + 1);
            Last() = value;
        }

        void Clear(){
            length_ = 0;
        }

        bool IsEmpty() const{
            return Length() == 0;
        }
    private:
        word length_;
        word capacity_;
        T* data_;

        void Resize(word newLen){
            if(newLen > capacity_){
                word newCap = RoundUpPowTwo(newLen);
                T* newData = reinterpret_cast<T*>(realloc(data_, newLen * sizeof(T)));
                data_ = newData;
                capacity_ = newCap;
            }
            length_ = newLen;
        }
    };
}

#endif //ESCHELLE_ARRAY_H
