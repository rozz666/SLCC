#ifndef SL_VM_FUNCTIONCALL_HPP
#define SL_VM_FUNCTIONCALL_HPP

#include <cstdint>
#include <string>
#include <vector>
#include <sl/vm/def.hpp>

namespace sl
{
namespace vm
{

class FunctionCall
{
public:

    FunctionCall(const std::string& name) : name_(name) { }

    const std::string& name() const { return name_; }

    FunctionCall& p(float_t f)
    {
        params_.insert(params_.begin(), (const byte *) &f, (const byte *) (&f + 1));
        return *this;
    }

    FunctionCall& p(int_t f)
    {
        params_.insert(params_.begin(), (const byte *) &f, (const byte *) (&f + 1));
        return *this;
    }

    const std::uint8_t *params() const { return &params_.front(); }
    std::uint16_t paramSize() const { return std::uint16_t(params_.size()); }

private:
    
    std::string name_;
    std::vector<std::uint8_t> params_;
};

}
}
#endif /* SL_VM_FUNCTIONCALL_HPP */
