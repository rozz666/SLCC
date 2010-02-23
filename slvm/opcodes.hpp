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
    ANDI        = 0x60,
    ORI         = 0x62,
    XORI        = 0x64,
    NOTI        = 0x66,

    I2F         = 0x70,
    F2I         = 0x71,
    
    JUMP        = 0x80,
    CALL        = 0x81,
    ENTER       = 0x82,
    LEAVE       = 0x83,
    RET         = 0x84,
    POP         = 0x85,

    CJUMP       = 0x86,

    LTI         = 0x90,
    LTF         = 0x91,
    LEI         = 0x98,
    LEF         = 0x99,
    GTI         = 0xa0,
    GTF         = 0xa1,
    GEI         = 0xa8,
    GEF         = 0xa9,
    EQI         = 0xb0,
    EQF         = 0xb1,
    NEQI        = 0xb8,
    NEQF        = 0xb9,

    INPI        = 0xfc,
    OUTI        = 0xfd
};

}

}

#endif /* SL_VM_OPCODES_HPP */