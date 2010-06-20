#include <ostream>
#include <iomanip>
#include <sl/FilePosition.hpp>

namespace sl
{

std::ostream& operator<<(std::ostream& os, const FilePosition& fp)
{
    std::ostream::sentry ok(os);

    if (ok)
    {
        auto flags = os.flags();

        os << std::dec << fp.file << '(' << fp.line << ", " << fp.column << ")";

        os.flags(flags);
    }

    return os;
}

}