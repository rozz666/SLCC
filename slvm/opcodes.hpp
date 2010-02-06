#ifndef SL_VM_OPCODES_HPP
#define SL_VM_OPCODES_HPP

namespace sl
{

namespace vm
{

enum Opcode
{
    NOP,
    CONST4,
    LOAD4,
    STORE4,
    DEREF4,
    POP4,
    ADDI,
    ADDF,
    SUBI,
    SUBF,
    MULI,
    MULF,
    DIVI,
    DIVF,
    MODI,
    MODF,
    I2F,
    F2I,

    CALL,
    ENTER,
    LEAVE,
    RET,
    JUMP

};

}

}

#endif /* SL_VM_OPCODES_HPP */