#include <vector>
#include <cmath>
#include <iostream>
#include "opcodes.hpp"
#include "vm.hpp"

namespace sl
{

namespace vm
{

void copy4(const void *src, void *dst)
{
    std::memcpy(dst, src, 4);
}

#define SLVM_OP4(type, op) \
{ \
    type a, b; \
    copy4(memory + sp, &b); \
    copy4(memory + sp + 4, &a); \
    a = a op b; \
    copy4(&a, memory + sp + 4); \
    sp += 4; \
}

#define SLVM_FUN4(type, f) \
{ \
    type a, b; \
    copy4(memory + sp, &b); \
    copy4(memory + sp + 4, &a); \
    a = f(a, b); \
    copy4(&a, memory + sp + 4); \
    sp += 4; \
}

void Program::execute()
{
    typedef std::int32_t int_t;
    typedef float float_t;

    static_assert(sizeof(int_t) == 4, "int_t must have 4 bytes");
    static_assert(sizeof(float_t) == 4, "float_t must have 4 bytes");

    const std::uint8_t *code = &code_.front();
    std::ptrdiff_t off = 0;
    std::uint8_t *memory = &memory_.front();
    std::ptrdiff_t sp = memory_.size();
    std::ptrdiff_t bp = sp;

    while (off != -1)
    {
        switch (code[off++])
        {
            case NOP:
                break;

            case CONST4:
                sp -= 4;
                copy4(code + off, memory + sp);
                off += 4;
                break;

            case LOAD4:
            {
                int_t addr;
                copy4(code + off, &addr);
                off += 4;
                sp -= 4;
                copy4(memory + bp + addr, memory + sp);
                break;
            }

            case STORE4:
            {
                int_t addr;
                copy4(code + off, &addr);
                off += 4;
                copy4(memory + sp, memory + bp + addr);
                sp += 4;
                break;
            }

            case DEREF4:
            {
                int_t addr;
                copy4(memory + sp, &addr);
                copy4(memory + addr, memory + sp);
                break;
            }

            case POP4:
                sp += 4;
                break;

            case ADDI:
                SLVM_OP4(int_t, +);
                break;

            case ADDF:
                SLVM_OP4(float_t, +);
                break;

            case SUBI:
                SLVM_OP4(int_t, -);
                break;

            case SUBF:
                SLVM_OP4(float_t, -);
                break;

            case MULI:
                SLVM_OP4(int_t, *);
                break;

            case MULF:
                SLVM_OP4(float_t, *);
                break;

            case DIVI:
                SLVM_OP4(int_t, /);
                break;

            case DIVF:
                SLVM_OP4(float_t, /);
                break;

            case MODI:
                SLVM_OP4(int_t, %);
                break;

            case MODF:
                SLVM_FUN4(float_t, std::fmod);
                break;

            case I2F:
            {
                int_t s;
                float_t d;
                copy4(memory + sp, &s);
                d = float_t(s);
                copy4(&d, memory +sp);
                break;
            }

            case F2I:
            {
                float_t s;
                int_t d;
                copy4(memory + sp, &s);
                d = int_t(s);
                copy4(&d, memory +sp);
                break;
            }

            case CALL:
            {
                int_t addr;
                copy4(memory + sp, &addr);
                int_t reta = off;
                copy4(&reta, memory + sp);
                off = addr;
                break;
            }

            case ENTER:
            {
                int_t ibp = bp;
                sp -= 4;
                copy4(&ibp, memory + sp);
                bp = sp;
                int_t d;
                copy4(code + off, &d);
                off += 4;
                sp -= d;
                break;
            }

            case LEAVE:
            {
                sp = bp;
                int_t ibp;
                copy4(memory + sp, &ibp);
                sp += 4;
                bp = ibp;
                break;
            }

            case RET:
            {
                int_t reta;
                copy4(memory + sp, &reta);
                sp += 4;
                off = reta;
                break;
            }

            case JUMP:
            {
                int_t addr;
                copy4(code + off, &addr);
                off = addr;
                break;
            }

            case INPI:
            {
                int_t addr, val;
                std::cin >> val;
                copy4(code + off, &addr);
                off += 4;
                copy4(&val, memory + bp + addr);
                break;
            }

            case OUTI:
            {
                int_t addr, val;
                copy4(code + off, &addr);
                off += 4;
                copy4(memory + bp + addr, &val);
                std::cout << val << std::endl;
                break;
            }
        }
    }
}

}
}