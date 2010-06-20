#include <sl/test.hpp>

sl::test::TestSuite<5> suite("References");

template <>
void sl::test::TestSuite<5>::run()
{
    test("Swap", "swaptest.sl")
        .match((values, 20, 50, 6.25, -0.5, 1, 0), (values, 50, 20, -0.5, 6.25, 0, 1))
        .match((values, -3, 10, -0.125, 0.125, 0, 1), (values, 10, -3, 0.125, -0.125, 1, 0));

    test("Immediate swap", "imswap.sl")
        .expect((values, 5, 6));

    test("Passing by reference", "references.sl")
        .match((values, 10, 10.5, 1), (values, 10, 10.5, 1))
        .match((values, -22, -0.5, 0), (values, -22, -0.5, 0));
}