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

    CodeAddr emit(Opcode oc)
    {
        code_.push_back(std::uint8_t(oc));
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