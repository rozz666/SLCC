#ifndef SL_VM_ENVIRONMENT_HPP
#define SL_VM_ENVIRONMENT_HPP

#include <vector>
#include <sl/vm/def.hpp>

namespace sl
{
namespace vm
{

class Environment
{
public:

    typedef std::vector<byte> Memory;

    Environment(Environment&& right) : sp_(right.sp_), bp_(right.bp_)
    {
        memory_.swap(right.memory_);
    }

    explicit Environment(std::size_t memorySize) : memory_(memorySize, 0xac), sp_(memorySize), bp_(sp_) { }

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
};

}
}
#endif /* SL_VM_ENVIRONMENT_HPP */
