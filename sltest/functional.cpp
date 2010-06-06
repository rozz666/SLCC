#include <sl/test.hpp>

sl::test::TestSuite<1> suite("Functional");

template <>
void sl::test::TestSuite<1>::run()
{
    test("Identity", "identity.sl")
        .match("10", "10\n")
        .match("15", "15\n")
        .match("-39", "-39\n");
}