#include <sl/test.hpp>

sl::test::TestSuite<5> suite("References");

template <>
void sl::test::TestSuite<5>::run()
{
    test("Swap", "swaptest.sl")
        .match("20 50 6.25 -0.5 1 0", "50\n20\n-0.5\n6.25\n0\n1\n")
        .match("-3 10 -0.125 0.125 0 1", "10\n-3\n0.125\n-0.125\n1\n0\n");

    test("Immediate swap", "imswap.sl")
        .match("", "5\n6\n");

    test("Passing by reference", "references.sl")
        .match("10 10.5 1", "10\n10.5\n1\n")
        .match("-22 -0.5 0", "-22\n-0.5\n0\n");
}