#include <algorithm>
#include <sl/ErrorLogger.hpp>

namespace sl
{

void ErrorLogger::print(std::ostream& os) const
{
    const std::string& filename = filename_;
    std::for_each(errors_.begin(), errors_.end(), [&os, &filename](const err::Message& m) { os << filename << m << '\n'; });
}

}