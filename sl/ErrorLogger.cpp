#include <algorithm>
#include <iterator>
#include <sl/ErrorLogger.hpp>

namespace sl
{

void ErrorLogger::print(std::ostream& os) const
{
    std::copy(errors_.begin(), errors_.end(), std::ostream_iterator<err::Message>(os, "\n"));
}

}