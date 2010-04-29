#ifndef SL_VM_CODEGENERATOR_HPP
#define SL_VM_CODEGENERATOR_HPP

#include <sl/assert.hpp>
#include <sl/vm/def.hpp>
#include <sl/vm/opcodes.hpp>
#include <sl/vm/BytecodeBuffer.hpp>

namespace sl
{
namespace vm
{

class CodeGenerator
{
public:

    BytecodeBuffer code;

	CodeAddr emit(Opcode oc)
    {
        code.push_back(byte(oc));
        return code.size() - 1;
    }

    template <typename T>
    CodeAddr emit(T val)
    {
        code.resize(code.size() + sizeof(val));
        std::memcpy(&code[code.size() - sizeof(val)], &val, sizeof(val));
        return code.size() - sizeof(val);
    }

    void emit(CodeAddr addr, Opcode oc)
    {
        SL_ASSERT(addr < code.size());
        code[addr] = byte(oc);
    }

	template <typename T>
    void emit(CodeAddr addr, T val)
    {
        SL_ASSERT(addr + sizeof(val) <= code.size());
        std::memcpy(&code[addr], &val, sizeof(val));
    }

    template <typename T>
    T read(CodeAddr addr)
    {
        T val;
        SL_ASSERT(addr + sizeof(val) <= code.size());
        std::memcpy(&val, &code[addr], sizeof(val));
        return val;
    }
};


}
}
#endif /* SL_VM_CODEGENERATOR_HPP */
