#ifndef SL_VM_CODEGENERATOR_HPP
#define SL_VM_CODEGENERATOR_HPP

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

    CodeAddr emit(Opcode oc)
    {
        code_.push_back(byte(oc));
        return code_.size() - 1;
    }

    template <typename T>
    CodeAddr emit(T val)
    {
        code_.resize(code_.size() + sizeof(val));
        std::memcpy(&code_[code_.size() - sizeof(val)], &val, sizeof(val));
        return code_.size() - sizeof(val);
    }

    template <typename T>
    void emit(CodeAddr addr, T val)
    {
        assert(addr + sizeof(val) <= code_.size());
        std::memcpy(&code_[addr], &val, sizeof(val));
    }

    template <typename T>
    T read(CodeAddr addr)
    {
        T val;
        assert(addr + sizeof(val) <= code_.size());
        std::memcpy(&val, &code_[addr], sizeof(val));
        return val;
    }

    byte& operator[](CodeAddr addr)
    {
        return code_[addr];
    }

    BytecodeBuffer& code() { return code_; }

private:

    BytecodeBuffer code_;
};


}
}
#endif /* SL_VM_CODEGENERATOR_HPP */
