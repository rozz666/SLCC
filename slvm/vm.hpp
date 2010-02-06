#ifndef SL_VM_VM_HPP
#define SL_VM_VM_HPP

#include <cstdint>
#include "opcodes.hpp"

namespace sl
{

namespace vm
{

class CodeGenerator
{
public:

    CodeGenerator& add(Opcode oc)
    {
        code_.push_back(oc);
        return *this;
    }

    CodeGenerator& add(Opcode oc, std::int32_t arg)
    {
        code_.push_back(oc);
        code_.resize(code_.size() + 4);
        std::memcpy(&code_[code_.size() - 4], &arg, 4);
        return *this;
    }

    CodeGenerator& add(Opcode oc, float arg)
    {
        code_.push_back(oc);
        code_.resize(code_.size() + 4);
        std::memcpy(&code_[code_.size() - 4], &arg, 4);
        return *this;
    }

    std::vector<std::uint8_t>& code() { return code_; }

private:

    std::vector<std::uint8_t> code_;
};

class Program
{
public:

    template <typename It>
    Program(std::size_t memorySize, It first, It last)
        : memory_(memorySize, 0xac), code_(first, last) { }

    Program(std::size_t memorySize, std::vector<std::uint8_t>&& code)
        : memory_(memorySize, 0xac), code_(code) { }

    void execute();

private:

    std::vector<std::uint8_t> memory_;
    std::vector<std::uint8_t> code_;
};



}
}

#endif /* SL_VM_VM_HPP */