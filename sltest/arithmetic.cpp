#include <sl/test.hpp>

sl::test::TestSuite<3> suite("Aritmetic");

template <>
void sl::test::TestSuite<3>::run()
{
    test("Plus", "plus.sl")
        .match((values, 10), (values, 10))
        .match((values, 0), (values, 0))
        .match((values, -10), (values, -10));

    test("Minus", "neg.sl")
        .match((values, 10), (values, -10))
        .match((values, 0), (values, 0))
        .match((values, -10), (values, 10));

    test("Add", "add.sl")
        .match((values, 1423, 6245), (values, 7668))
        .match((values, -36, 23), (values, -13))
        .match((values, -55, -66), (values, -121));

    test("Sub", "sub.sl")
        .match((values, 1423, 6245), (values, -4822))
        .match((values, -36, 23), (values, -59))
        .match((values, -55, -66), (values, 11));

    test("Mul", "mul.sl")
        .match((values, 1423, 6245), (values, 8886635))
        .match((values, -36, 23), (values, -828))
        .match((values, -55, -66), (values, 3630));

    test("Div", "div.sl")
        .match((values, 123, 5), (values, 24))
        .match((values, -36, 3), (values, -12))
        .match((values, -55, -6), (values, 9));

    test("Mod", "mod.sl")
        .match((values, 123, 5), (values, 3))
        .match((values, -37, 3), (values, -1))
        .match((values, -55, -6), (values, -1));
}