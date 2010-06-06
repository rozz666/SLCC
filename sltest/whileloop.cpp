#include <sl/test.hpp>

sl::test::TestSuite<8> suite("while loop");

template <>
void sl::test::TestSuite<8>::run()
{
    test("while loop", "whiletest.sl")
        .match("50", "50\n")
        .match("0", "0\n");
}