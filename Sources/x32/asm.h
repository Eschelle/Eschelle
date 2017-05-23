#ifndef ESCHELLE_X32ASM_H
#define ESCHELLE_X32ASM_H

#include "../common.h"
#include "../vmemory.h"
#include "../asm.h"
#include "constants.h"

namespace Eschelle{
    class Immediate{
    public:
        explicit Immediate(int32_t value):
                value_(value){}
        Immediate(const Immediate& other):
                value_(other.value_){}

        int32_t GetValue() const{
            return value_;
        }
    private:
        const int32_t value_;
    };

    class Operand{
    private:
        uint8_t length_;
        uint8_t encoding_[6];
        uint8_t padding_;

        explicit Operand(Register reg){
            SetModRM(3, reg);
        }

        uint8_t EncodingAt(word index) const{
            return encoding_[index];
        }

        bool IsRegister(Register reg) const{
            return ((encoding_[0] & 0xF8) == 0xC0) &&
                    ((encoding_[0] & 0x07) == reg);
        }

        friend class Assembler;
    protected:
        Operand(): length_(0){}

        void SetModRM(int mod, Register rm){
            encoding_[0] = (mod << 6) | rm;
            length_ = 1;
        }

        void SetSIB(ScaleFactor scale, Register index, Register base){
            encoding_[1] = (scale << 6) | (index << 3) | base;
            length_ = 2;
        }

        void SetDisp8(int8_t disp){
            encoding_[length_++] = static_cast<uint8_t>(disp);
        }

        void SetDisp32(int32_t disp){
            memmove(&encoding_[length_], &disp, sizeof(disp));
            length_ += sizeof(disp);
        }
    public:
        uint8_t GetMod() const{
            return (EncodingAt(0) >> 6) & 3;
        }

        Register GetRm() const{
            return static_cast<Register>(EncodingAt(0) & 0x7);
        }

        Register GetIndex() const{
            return static_cast<Register>((EncodingAt(1) >> 6) & 0x3);
        }

        Register GetBase() const{
            return static_cast<Register>(EncodingAt(1) & 0x7);
        }

        int8_t GetDisp8() const{
            return static_cast<int8_t>(encoding_[length_ - 1]);
        }

        int32_t GetDisp32() const{
            
        }
    };
}

#endif //ESCHELLE_X32ASM_H
