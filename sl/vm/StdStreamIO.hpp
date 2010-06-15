#ifndef SL_VM_STDSTREAMIO_HPP
#define SL_VM_STDSTREAMIO_HPP

#include <iosfwd>
#include <sl/def.hpp>

namespace sl
{
namespace vm
{

class StdStreamIO
{
public:

    StdStreamIO(std::istream& is, std::ostream& os);

    void putInt(int_t val);
    void putFloat(float_t val);
    int_t getInt();
    float_t getFloat();

private:

    std::istream *is_;
    std::ostream *os_;
};

}
}

#endif /* SL_VM_STDSTREAMIO_HPP */