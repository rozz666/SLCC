#ifndef SL_VM_PROGRAM_HPP
#define SL_VM_PROGRAM_HPP

#include <sl/vm/def.hpp>
#include <sl/vm/Environment.hpp>
#include <sl/vm/FunctionCall.hpp>
#include <sl/vm/Module.hpp>

namespace sl
{
namespace vm
{

class Program
{
public:

    Program(Module&& module, Environment&& env)
        : module_(module), env_(env)
    {
        if (!module_.hasFunction("main$"))
        {
            throw std::runtime_error("No main function in program!");
        }
    }

    void execute()
    {
        module_.call<void>(FunctionCall("main$"), env_);
    }

private:

    Module module_;
    Environment env_;
};

}
}
#endif /* SL_VM_PROGRAM_HPP */
