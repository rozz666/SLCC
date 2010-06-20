#ifndef SL_ASSERT_HPP
#define SL_ASSERT_HPP

#ifndef SL_UNIT_TESTS
#include <cassert>

#define SL_ASSERT(cond) assert(cond)

#else /* SL_UNIT_TESTS */
#include <stdexcept>
namespace sl
{

class AssertionFailed : public std::logic_error
{
public:

    AssertionFailed(const char *msg, const char *file, unsigned line);
};

}

#define SL_ASSERT(cond) (!(cond) ? throw sl::AssertionFailed(#cond, __FILE__, __LINE__) : (void) 0)

#endif /* SL_UNIT_TESTS */

#endif /* SL_ASSERT_HPP */