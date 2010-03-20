#ifndef SL_VM_MODULE_HPP
#define SL_VM_MODULE_HPP

#include <cassert>
#include <unordered_map>
#include <sl/vm/BytecodeBuffer.hpp>

namespace sl
{
namespace vm
{

class Module
{
public:

    Module(Module&& right)
    {
        code_.swap(right.code_);
        functions_.swap(right.functions_);
    }

    Module(BytecodeBuffer&& code) : code_(code) { }

    void registerFunction(CodeAddr addr, const std::string& name)
    {
        functions_.insert(std::make_pair(name, addr));
    }

    template <typename retT>
    retT call(const FunctionCall& fc, Environment& env) const
    {
        auto f = functions_.find(fc.name());

        assert(f != functions_.end());

        env.sp(env.sp() - fc.paramSize() - sizeof(retT) - sizeof(CodeAddr));

        if (fc.paramSize()) std::memcpy(&env[env.sp() + sizeof(CodeAddr)], fc.params(), fc.paramSize());

        CodeAddr retAddr = ~CodeAddr(0);
        std::memcpy(&env[env.sp()], &retAddr, sizeof(retAddr));

        env.execute(&code_.front(), f->second);
        env.sp(env.sp() + fc.paramSize());

        retT ret;
        std::memcpy(&ret, &env[env.sp()], sizeof(ret));

        env.sp(env.sp() + sizeof(retT));

        return ret;
    }

private:

    BytecodeBuffer code_;
    std::unordered_map<std::string, CodeAddr> functions_;
};

}
}
#endif /* SL_VM_MODULE_HPP */
