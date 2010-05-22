#include <istream>
#include <ostream>
#include <cstring>
#include <sstream>
#include <sl/def.hpp>
#include <sl/vm/opcodes.hpp>
#include <sl/vm/Environment.hpp>

namespace sl
{

namespace vm
{

namespace
{

void copy1(const void *src, void *dst)
{
    std::memcpy(dst, src, 1);
}


void copy2(const void *src, void *dst)
{
    std::memcpy(dst, src, 2);
}


void copy4(const void *src, void *dst)
{
    std::memcpy(dst, src, 4);
}

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

#define SLVM_REL_OP4(type, op) \
{ \
    type a, b; \
    copy4(memory + sp, &b); \
    copy4(memory + sp + 4, &a); \
    int_t r = a op b; \
    copy4(&r, memory + sp + 4); \
    sp += 4; \
}


void Environment::execute(const std::uint8_t *code, CodeAddr addr)
{
    std::ptrdiff_t off = addr;
    std::uint8_t *memory = &memory_.front();
    std::ptrdiff_t sp = sp_;
    std::ptrdiff_t bp = bp_;

    while (off != -1 && code[off] != EXIT) // TODO: remove first condition
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

            case LADDR:
            {
                std::int16_t laddr;
                copy2(code + off, &laddr);
                off += 2;
                sp -= 4;
                std::int32_t addr = bp + laddr;
                copy4(&addr, memory + sp);
                break;
            }

            case LOAD4:
            {
                std::int32_t addr;
                copy4(memory + sp, &addr);
                copy4(memory + addr, memory + sp);
                break;
            }

            case LLOAD4:
            {
                std::int16_t addr;
                copy2(code + off, &addr);
                off += 2;
                sp -= 4;
                copy4(memory + bp + addr, memory + sp);
                break;
            }

            case STORE4:
            {
                std::int32_t addr;
                copy4(memory + sp + 4, &addr);
                copy4(memory + sp, memory + addr);
                sp += 8;
                break;
            }

            case LSTORE4:
            {
                std::int16_t addr;
                copy2(code + off, &addr);
                off += 2;
                copy4(memory + sp, memory + bp + addr);
                sp += 4;
                break;
            }

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

            case NEGI:
            {
                int_t v;
                copy4(memory + sp, &v);
                v = -v;
                copy4(&v, memory + sp);
                break;
            }
                
            case NEGF:
            {
                float_t v;
                copy4(memory + sp, &v);
                v = -v;
                copy4(&v, memory + sp);
                break;
            }

            case ANDI:
                SLVM_OP4(int_t, &);
                break;

            case ORI:
                SLVM_OP4(int_t, |);
                break;

            case XORI:
                SLVM_OP4(int_t, ^);
                break;

            case NOTI:
            {
                int_t v;
                copy4(memory + sp, &v);
                v = ~v;
                copy4(&v, memory + sp);
                break;
            }

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

            case JUMP:
            {
                int_t addr;
                copy4(memory + sp, &addr);
                sp += 4;
                off = addr;
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
                std::uint16_t d;
                copy2(code + off, &d);
                off += 2;
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

            case POP:
            {
                std::uint8_t n;
                copy1(code + off, &n);
                ++off;
                sp += n;

                break;
            }

            case CJUMP:
            {
                int_t addr;
                int_t cond;
                copy4(memory + sp, &cond);
                copy4(memory + sp + 4, &addr);
                sp += 8;
                if (cond) off = addr;
                break;
            }

            case LTI:
                SLVM_REL_OP4(int_t, <);
                break;

            case LTF:
                SLVM_REL_OP4(float_t, <);
                break;

            case LEI:
                SLVM_REL_OP4(int_t, <=);
                break;

            case LEF:
                SLVM_REL_OP4(float_t, <=);
                break;

            case GTI:
                SLVM_REL_OP4(int_t, >);
                break;

            case GTF:
                SLVM_REL_OP4(float_t, >);
                break;

            case GEI:
                SLVM_REL_OP4(int_t, >=);
                break;

            case GEF:
                SLVM_REL_OP4(float_t, >=);
                break;

            case EQI:
                SLVM_REL_OP4(int_t, ==);
                break;

            case EQF:
                SLVM_REL_OP4(float_t, ==);
                break;

            case NEQI:
                SLVM_REL_OP4(int_t, !=);
                break;

            case NEQF:
                SLVM_REL_OP4(float_t, !=);
                break;

            case INPI:
            {
                int_t val;
                *inputStream >> val;
                sp -= 4;
                copy4(&val, memory + sp);
                break;
            }

            case INPF:
            {
                float_t val;
                *inputStream >> val;
                sp -= 4;
                copy4(&val, memory + sp);
                break;
            }

            case OUTI:
            {
                int_t val;
                copy4(memory + sp, &val);
                sp += 4;
                *outputStream << val << std::endl;
                break;
            }

            default:
            {
                std::ostringstream os;
                os << "Invalid instruction 0x" << unsigned(code[off - 1]);
                throw InvalidInstruction(os.str());
            }
        }
    }

    sp_ = sp;
    bp_ = bp;
}

}
}