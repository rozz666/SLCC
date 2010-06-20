#ifndef SL_TEST_CONSOLE_HPP
#define SL_TEST_CONSOLE_HPP

#include <ostream>

namespace sl
{
namespace test
{

std::ostream& rc(std::ostream& os);
std::ostream& green(std::ostream& os);
std::ostream& red(std::ostream& os);

}
}

#endif /* SL_TEST_CONSOLE_HPP */