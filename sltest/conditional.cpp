#include <sl/test.hpp>

sl::test::TestSuite<7> suite("Conditional");

template <>
void sl::test::TestSuite<7>::run()
{
    test("if", "iftest.sl")
        .match("-5 5", "5\n5\n5\n5\n")
        .match("5 -5", "5\n5\n5\n5\n")
        .match("10 20", "20\n20\n20\n20\n")
        .match("20 10", "20\n20\n20\n20\n");

}