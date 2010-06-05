#ifndef SL_VM_MODULE_HPP
#define SL_VM_MODULE_HPP

#include <unordered_map>
#include <sl/assert.hpp>
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

    bool hasFunction(const std::string& name)
    {
        return functions_.find(name) != functions_.end();
    }

    template <typename retT>
    retT call(const FunctionCall& fc, Environment& env) const
    {
        auto f = functions_.find(fc.name());

        SL_ASSERT(f != functions_.end());

        env.sp(env.sp() - fc.paramSize() - sizeof(retT) - sizeof(CodeAddr));

        if (fc.paramSize()) std::memcpy(&env.memory()[env.sp() + sizeof(CodeAddr)], fc.params(), fc.paramSize());

        CodeAddr retAddr = ~CodeAddr(0);
        std::memcpy(&env.memory()[env.sp()], &retAddr, sizeof(retAddr));

        env.execute(&code_.front(), f->second);
        env.sp(env.sp() + fc.paramSize());

        retT ret;
        std::memcpy(&ret, &env.memory()[env.sp()], sizeof(ret));

        env.sp(env.sp() + sizeof(retT));

        return ret;
    }

    template <>
    void call<void>(const FunctionCall& fc, Environment& env) const
    {
        auto f = functions_.find(fc.name());

        SL_ASSERT(f != functions_.end());

        env.sp(env.sp() - fc.paramSize() - sizeof(CodeAddr));

        if (fc.paramSize()) std::memcpy(&env.memory()[env.sp() + sizeof(CodeAddr)], fc.params(), fc.paramSize());

        CodeAddr retAddr = ~CodeAddr(0);
        std::memcpy(&env.memory()[env.sp()], &retAddr, sizeof(retAddr));

        env.execute(&code_.front(), f->second);
        env.sp(env.sp() + fc.paramSize());
    }

private:

    BytecodeBuffer code_;
    std::unordered_map<std::string, CodeAddr> functions_;
};

}
}
#endif /* SL_VM_MODULE_HPP */
