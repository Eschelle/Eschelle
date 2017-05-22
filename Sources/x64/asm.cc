#include "../common.h"
#if defined(ARCH_IS_X64)
#include "asm.h"

namespace Eschelle{
    void Assembler::EmitOperand(int rm, Operand &oper){
        const word length = oper.length_;
        EmitUint8(static_cast<uint8_t>(oper.encoding_[0] + (rm << 3)));
        for(word i = 1; i < length; i++){
            EmitUint8(oper.encoding_[i]);
        }
    }

    void Assembler::EmitImmediate(const Immediate &imm){
        EmitInt64(imm.value_);
    }

    void Assembler::addq(Register dst, Register src){
        Operand oper(src);
        EmitOperandRex(dst, oper, REX_W);
        EmitUint8(0x03);
        EmitOperand(dst & 7, oper);
    }

    void Assembler::addq(Register dst, Address &src){
        EmitOperandRex(dst, src, REX_W);
        EmitUint8(0x03);
        EmitOperand(dst & 7, src);
    }

    void Assembler::addq(Register dst, const Immediate &src){
        movq(TMP, src);
        addq(dst, TMP);
    }

    void Assembler::addq(Address &dst, Register src){
        EmitOperandRex(src, dst, REX_W);
        EmitUint8(0x01);
        EmitOperand(src & 7, dst);
    }

    void Assembler::addq(Address &dst, const Immediate &src){
        movq(TMP, src);
        addq(dst, TMP);
    }

    void Assembler::movq(Register dst, const Immediate &src){
        EmitRegisterRex(dst, REX_W);
        EmitUint8(static_cast<uint8_t>(0xB8 | (dst & 7)));
        EmitImmediate(src);
    }

    void Assembler::ret(){
        EmitUint8(0xC3);
    }
}

#endif // ARCH_IS_X64