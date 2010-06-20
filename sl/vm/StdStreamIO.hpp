#ifndef SL_VM_STDSTREAMIO_HPP
#define SL_VM_STDSTREAMIO_HPP

#include <iosfwd>
#include <sl/def.hpp>
#include <sl/vm/StreamIO.hpp>

namespace sl
{
namespace vm
{

class StdStreamIO : public vm::StreamIO
{
public:

    StdStreamIO(std::istream& is, std::ostream& os);

    virtual void putInt(int_t val);
    virtual void putFloat(float_t val);
    virtual int_t getInt();
    virtual float_t getFloat();

private:

    std::istream *is_;
    std::ostream *os_;
};

}
}

#endif /* SL_VM_STDSTREAMIO_HPP */