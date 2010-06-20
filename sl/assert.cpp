#include <sl/assert.hpp>

#ifdef SL_UNIT_TESTS

#include <sstream>

namespace sl
{

namespace
{

std::string formatMessage(const char *msg, const char *file, unsigned line)
{
    std::ostringstream os;

    os << "Assertion failed: " << msg << " at " << file << "(" << line << ")";

    return os.str();
}

}

AssertionFailed::AssertionFailed(const char *msg, const char *file, unsigned line) : std::logic_error(formatMessage(msg, file, line)) { }

}

#endif /* SL_UNIT_TESTS */