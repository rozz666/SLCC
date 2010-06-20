#include <istream>
#include <ostream>
#include <sl/vm/StdStreamIO.hpp>

namespace sl
{
namespace vm
{

StdStreamIO::StdStreamIO(std::istream& is, std::ostream& os) : is_(&is), os_(&os) { }

void StdStreamIO::putInt(int_t val)
{
    *os_ << val << std::endl;
}

void StdStreamIO::putFloat(float_t val)
{
    *os_ << val << std::endl;
}

int_t StdStreamIO::getInt()
{
    int_t val;

    *is_ >> val;

    return val;
}

float_t StdStreamIO::getFloat()
{
    float_t val;

    *is_ >> val;

    return val;
}

}
}