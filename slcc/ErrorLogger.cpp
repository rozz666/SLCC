#include <algorithm>
#include "ErrorLogger.hpp"

namespace sl
{

namespace err
{

std::ostream& operator<<(std::ostream& os, const Message& m)
{
    return os << '(' << m.pos.line << ", " << m.pos.column << "): error " << m.id << ": " << m.text;
}

}

void ErrorLogger::sort()
{
    std::sort(errors_.begin(), errors_.end(), [](const err::Message& l, const err::Message& r) { return l.pos < r.pos; });
}

void ErrorLogger::print(std::ostream& os)
{
    const std::string& filename = filename_;
    std::for_each(errors_.begin(), errors_.end(), [&os, &filename](const err::Message& m) { os << filename << m << '\n'; });
}

}