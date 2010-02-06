#ifndef SL_VM_VM_HPP
#define SL_VM_VM_HPP

#include <cstdint>
#include "opcodes.hpp"

namespace sl
{

namespace vm
{

void execute(const std::uint8_t *code, std::ptrdiff_t off);

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

    const std::vector<std::uint8_t> code() const { return code_; }

private:

    std::vector<std::uint8_t> code_;
};

class Module
{
public:


};

}
}

#endif /* SL_VM_VM_HPP */