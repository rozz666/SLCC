#include <algorithm>
#include <sl/ErrorLogger.hpp>

namespace sl
{

namespace err
{

std::ostream& operator<<(std::ostream& os, const Message& m)
{
    return os << '(' << m.pos.line << ", " << m.pos.column << "): error " << m.id << ": " << m.text;
}

}

void ErrorLogger::print(std::ostream& os) const
{
    const std::string& filename = filename_;
    std::for_each(errors_.begin(), errors_.end(), [&os, &filename](const err::Message& m) { os << filename << m << '\n'; });
}

}