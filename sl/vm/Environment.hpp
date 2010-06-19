#ifndef SL_VM_ENVIRONMENT_HPP
#define SL_VM_ENVIRONMENT_HPP

#include <iosfwd>
#include <vector>
#include <sl/vm/def.hpp>
#include <sl/vm/StreamIO.hpp>

namespace sl
{
namespace vm
{

class RuntimeError : public std::runtime_error
{
public:
    RuntimeError(const std::string& message) : std::runtime_error(message) { }
};

class InvalidInstruction : public RuntimeError
{
public:
    InvalidInstruction(const std::string& message) : RuntimeError(message) { }
};

class Environment
{
public:

    typedef std::vector<byte> Memory;

    Environment(Environment&& right) : sp_(right.sp_), bp_(right.bp_)
    {
        memory_.swap(right.memory_);
    }

    explicit Environment(std::size_t memorySize, StreamIO& streamIO)
        : sp_(memorySize), bp_(sp_), memory_(memorySize, 0xac), streamIO_(&streamIO) { }

    DataAddr sp() const { return sp_; }
    void sp(DataAddr val) { sp_ = val; }

    DataAddr bp() const { return bp_; }
    void bp(DataAddr val) { bp_ = val; }

    Memory& memory() { return memory_; }
    const Memory& memory() const { return memory_; }

    void execute(const byte *code, CodeAddr addr);

private:

    DataAddr sp_, bp_;
    Memory memory_;
    StreamIO *streamIO_;
};

}
}
#endif /* SL_VM_ENVIRONMENT_HPP */
