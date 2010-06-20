#include <sl/test.hpp>

sl::test::TestSuite<8> suite("while loop");

template <>
void sl::test::TestSuite<8>::run()
{
    test("while loop", "whiletest.sl")
        .match((values, 50), (values, 50))
        .match((values, 0), (values, 0));
}