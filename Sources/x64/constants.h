#ifndef ESCHELLE_CONSTANTS_H
#define ESCHELLE_CONSTANTS_H

namespace Eschelle{
    enum Register{
        RAX = 0,
        RCX = 1,
        RDX = 2,
        RBX = 3,
        RSP = 4,
        RBP = 5,
        RSI = 6,
        RDI = 7,
        R8 = 8,
        R9 = 9,
        R10 = 10,
        R11 = 11,
        R12 = 12,
        R13 = 13,
        R14 = 14,
        R15 = 15,
        kNumberOfCpuRegisters = 16,
        kNoRegister = -1
    };

    enum FpuRegister{
        XMM0 = 0,
        XMM1 = 1,
        XMM2 = 2,
        XMM3 = 3,
        XMM4 = 4,
        XMM5 = 5,
        XMM6 = 6,
        XMM7 = 7,
        XMM8 = 8,
        XMM9 = 9,
        XMM10 = 10,
        XMM11 = 11,
        XMM12 = 12,
        XMM13 = 13,
        XMM14 = 14,
        XMM15 = 15,
        kNumberOfFpuRegisters = 16
    };

    enum RexBits{
        REX_NONE = 0,
        REX_B = 1 << 0,
        REX_X = 1 << 1,
        REX_R = 1 << 2,
        REX_W = 1 << 3,
        REX_PREFIX = 1 << 6
    };

    enum ScaleFactor{
        TIMES_1 = 0,
        TIMES_2 = 1,
        TIMES_4 = 2,
        TIMES_8 = 3,
        TIMES_16 = 4
    };

    enum Conditional{
        OVERFLOW = 0,
        NO_OVERFLOW = 1,
        BELOW = 2,
        ABOVE_EQUAL = 3,
        EQUAL = 4,
        NOT_EQUAL = 5,
        BELOW_EQUAL = 6,
        ABOVE = 7,
        SIGN = 8,
        NOT_SIGN = 9,
        PARITY_EVEN = 10,
        PARITY_ODD = 11,
        LESS = 12,
        GREATER_EQUAL = 13,
        LESS_EQUAL = 14,
        GREATER = 15,
        ZERO = EQUAL,
        NOT_ZERO = NOT_EQUAL,
        NEGATIVE = SIGN,
        POSITIVE = NOT_SIGN,
        CARRY = BELOW,
        NOT_CARRY = ABOVE_EQUAL
    };

    static const Register TMP = R11;
    static const Register CODE_REG = R12;
    static const Register SPREG = RSP;
    static const Register FPREG = RBP;

    static const FpuRegister FPU_TMP = XMM0;
}

#endif //ESCHELLE_CONSTANTS_H