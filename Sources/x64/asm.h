#ifndef ESCHELLE_X64ASM_H
#define ESCHELLE_X64ASM_H

#include <cstdint>
#include "../common.h"
#include "../vmemory.h"
#include "../asm.h"
#include "constants.h"

namespace Eschelle{
    class Label{
    private:
        word pos_;
        word unresolved_;

        void BindTo(word pos){
            pos_ = -pos - kWordSize;
        }

        void LinkTo(word pos){
            pos_ = pos + kWordSize;
        }

        friend class Assembler;
    public:
        Label(): pos_(0), unresolved_(0){}
        ~Label(){}

        word Position() const{
            return -pos_ - kWordSize;
        }

        word LinkPosition() const{
            return pos_ - kWordSize;
        }

        bool IsBound() const{
            return pos_ < 0;
        }

        bool IsLinked() const{
            return pos_ > 0;
        }
    };

    class Immediate{
    private:
        const int64_t value_;

        friend class Assembler;
    public:
        explicit Immediate(int64_t value):
                value_(value){}
        Immediate(const Immediate& other):
                value_(other.value_){}

        int64_t Value() const{
            return value_;
        }
    };

    class Operand{
    private:
        uint8_t length_;
        uint8_t rex_bits_;
        uint8_t encoding_[6];

        explicit Operand(Register reg):
                rex_bits_(REX_NONE){
            SetModRM(3, reg);
        }

        uint8_t EncodingAt(word index) const{
            return encoding_[index];
        }

        bool IsRegister(Register reg) const{
            return ((reg > 7 ? 1 : 0) == (rex_bits_ & REX_B))
                && ((EncodingAt(0) & 0xF8) == 0xC0)
                    && ((EncodingAt(0) & 0x07) == reg);
        }

        friend class Assembler;
    protected:
        Operand(): length_(0), rex_bits_(REX_NONE){}

        void SetModRM(int mod, Register rm){
            if((rm > 7) && !((rm == R12) && (mod != 3))){
                rex_bits_ |= REX_B;
            }
            encoding_[0] = (uint8_t) ((mod << 6) | (rm & 7));
            length_ = 1;
        }

        void SetSIB(ScaleFactor scale, Register index, Register base){
            if(base > 7) rex_bits_ |= REX_B;
            if(index > 7) rex_bits_ |= REX_X;
            encoding_[1] = (scale << 6) | ((index & 7) << 3) | (base & 7);
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
        uint8_t GetRex() const{
            return rex_bits_;
        }

        uint8_t GetMod() const{
            return static_cast<uint8_t>((EncodingAt(0) >> 6) & 3);
        }

        Register GetRM() const{
            int rm_rex = (rex_bits_ & REX_B) << 3;
            return static_cast<Register>(rm_rex + (EncodingAt(0) & 7));
        }

        Register GetIndex() const{
            int index_rex = (rex_bits_ & REX_X) << 2;
            return static_cast<Register>(index_rex + ((EncodingAt(1) >> 3) & 7));
        }

        Register GetBase() const{
            int base_rex = (rex_bits_ & REX_B) << 3;
            return static_cast<Register>(base_rex + (EncodingAt(1) & 7));
        }

        Operand(const Operand& other):
                length_(other.length_),
                rex_bits_(other.rex_bits_){
            memmove(encoding_, other.encoding_, other.length_);
        }

        Operand& operator=(const Operand& other){
            length_ = other.length_;
            rex_bits_ = other.rex_bits_;
            memmove(encoding_, other.encoding_, other.length_);
            return *this;
        }
    };

    class Address : public Operand{
    private:
        Address(Register base, int32_t disp, bool fixed){
            SetModRM(2, base);
            if((base & 7) == RSP) SetSIB(TIMES_1, RSP, base);
            SetDisp32(disp);
        }
    public:
        Address(Register base, int32_t disp){
            if((disp == 0) && (base & 7) != RBP){
                SetModRM(0, base);
                if((base & 7) == RSP) SetSIB(TIMES_1, RSP, base);
            } else{
                SetModRM(2, base);
                if((base & 7) == RSP) SetSIB(TIMES_1, RSP, base);
                SetDisp32(disp);
            }
        }

        Address(Register index, ScaleFactor scale, int32_t disp){
            SetModRM(0, RSP);
            SetSIB(scale, index, RBP);
            SetDisp32(disp);
        }

        Address(Register base, Register index, ScaleFactor scale, int32_t disp){
            if((disp == 0) && (base & 7) != RBP){
                SetModRM(0, RSP);
                SetSIB(scale, index, base);
            } else{
                SetModRM(2, RSP);
                SetSIB(scale, index, base);
                SetDisp32(disp);
            }
        }

        Address(const Address& other): Operand(other){}

        Address& operator=(const Address& other){
            Operand::operator=(other);
            return *this;
        }
    };

    class Assembler{
    private:
        AssemblerBuffer buffer_;

        void EmitImmediate(const Immediate& imm);
        void EmitOperand(int rm, Operand& oper);

        inline void EmitUint8(uint8_t value){
            buffer_.Emit<uint8_t>(value);
        }

        inline void EmitOperandRex(int rm, Operand& oper, uint8_t rex){
            rex |= (rm > 7 ? REX_R : REX_NONE) | oper.GetRex();
            if(rex != REX_NONE) EmitUint8(REX_PREFIX|rex);
        }

        inline void EmitRegisterRex(Register reg, uint8_t rex){
            rex |= (reg > 7 ? REX_B : REX_NONE);
            if(rex != REX_NONE) EmitUint8(REX_PREFIX|rex);
        }

        inline void EmitInt32(int32_t value){
            buffer_.Emit<int32_t>(value);
        }

        inline void EmitInt64(int64_t value){
            buffer_.Emit<int64_t>(value);
        }
    public:
        explicit Assembler(): buffer_(){}
        ~Assembler(){}

        void addq(Register dst, Register src);
        void addq(Register dst, const Immediate& src);
        void addq(Register dst, Address& src);
        void addq(Address& dst, Register src);
        void addq(Address& dst, const Immediate& src);

        void movq(Register dst, const Immediate& src);

        void ret();

        void Compile(MemoryRegion** region){
            *region = new MemoryRegion(buffer_.Size());
            buffer_.Commit(*region);
        }
    };
}

#endif //ESCHELLE_X64ASM_H