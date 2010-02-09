#ifndef SL_VM_OPCODES_HPP
#define SL_VM_OPCODES_HPP

namespace sl
{

namespace vm
{

enum Opcode
{
    NOP         = 0x00,
    CONST4      = 0x0a,
    LOAD4       = 0x12,
    STORE4      = 0x1a,
    ADDI        = 0x30,
    ADDF        = 0x31,
    SUBI        = 0x38,
    SUBF        = 0x39,
    MULI        = 0x40,
    MULF        = 0x41,
    DIVI        = 0x48,
    DIVF        = 0x49,
    MODI        = 0x50,
    MODF        = 0x51,
    NEGI        = 0x58,
    NEGF        = 0x59,
    I2F         = 0x70,
    F2I         = 0x71,
    
    JUMP        = 0x80,
    CALL        = 0x81,
    ENTER       = 0x82,
    LEAVE       = 0x83,
    RET         = 0x84,
    POP         = 0x85,

    INPI        = 0xfc,
    OUTI        = 0xfd
};

}

}

#endif /* SL_VM_OPCODES_HPP */