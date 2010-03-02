#ifndef SL_VM_VM_HPP
#define SL_VM_VM_HPP

#include <cstdint>
#include <map>
#include <string>
#include <cassert>
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

class FunctionCall
{
public:

    FunctionCall(const std::string& name) : name_(name) { }

    const std::string& name() const { return name_; }

    FunctionCall& p(float f)
    {
        params_.insert(params_.begin(), (const std::uint8_t *) &f, (const std::uint8_t *) (&f + 1));
        return *this;
    }

    FunctionCall& p(int f)
    {
        params_.insert(params_.begin(), (const std::uint8_t *) &f, (const std::uint8_t *) (&f + 1));
        return *this;
    }

    const std::uint8_t *params() const { return &params_.front(); }
    std::uint16_t paramSize() const { return std::uint16_t(params_.size()); }

private:
    
    std::string name_;
    std::vector<std::uint8_t> params_;
};

class Environment
{
public:

    Environment(Environment&& right) : sp_(right.sp_), bp_(right.bp_)
    {
        memory_.swap(right.memory_);
    }

    explicit Environment(std::size_t memorySize) : memory_(memorySize, 0xac), sp_(memorySize), bp_(sp_) { }

    std::ptrdiff_t sp() const { return sp_; }
    void sp(std::ptrdiff_t val) { sp_ = val; }

    std::ptrdiff_t bp() const { return bp_; }
    void bp(std::ptrdiff_t val) { bp_ = val; }

    const std::uint8_t& operator[](std::size_t idx) const { return memory_[idx]; }
    std::uint8_t& operator[](std::size_t idx) { return memory_[idx]; }

    void execute(const std::uint8_t *code, CodeAddr addr);

private:

    std::ptrdiff_t sp_, bp_;
    std::vector<std::uint8_t> memory_;
};

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
    std::map<std::string, CodeAddr> functions_;
};

class Program
{
public:

    Program(Module&& module, Environment&& env)
        : module_(module), env_(env) { }

    int execute()
    {
        return module_.call<int>(FunctionCall("main$"), env_);
    }

private:

    Module module_;
    Environment env_;
};

}
}

#endif /* SL_VM_VM_HPP */