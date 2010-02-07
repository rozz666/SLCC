#ifndef SL_VM_VM_HPP
#define SL_VM_VM_HPP

#include <cstdint>
#include "opcodes.hpp"

namespace sl
{

namespace vm
{

typedef std::vector<std::uint8_t> BytecodeBuffer;
typedef std::uint32_t CodeAddr;
typedef std::uint32_t BPAddr;

class CodeGenerator
{
public:

    CodeAddr add(Opcode oc)
    {
        code_.push_back(oc);
        return code_.size() - 1;
    }

    CodeAddr add(Opcode oc, std::int32_t arg)
    {
        code_.push_back(oc);
        code_.resize(code_.size() + 4);
        std::memcpy(&code_[code_.size() - 4], &arg, 4);
        return code_.size() - 5;
    }

    CodeAddr add(Opcode oc, float arg)
    {
        code_.push_back(oc);
        code_.resize(code_.size() + 4);
        std::memcpy(&code_[code_.size() - 4], &arg, 4);
        return code_.size() - 5;
    }

    std::uint8_t& operator[](CodeAddr addr)
    {
        return code_[addr];
    }

    BytecodeBuffer& code() { return code_; }

private:

    BytecodeBuffer code_;
};

class Program
{
public:

    template <typename It>
    Program(std::size_t memorySize, It first, It last)
        : memory_(memorySize, 0xac), code_(first, last) { }

    Program(std::size_t memorySize, BytecodeBuffer&& code)
        : memory_(memorySize, 0xac), code_(code) { }

    void execute();

private:

    std::vector<std::uint8_t> memory_;
    BytecodeBuffer code_;
};



}
}

#endif /* SL_VM_VM_HPP */