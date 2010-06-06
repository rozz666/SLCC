#include <sl/test.hpp>

sl::test::TestSuite<4> suite("Logical");

template <>
void sl::test::TestSuite<4>::run()
{
    test("Logical Not", "lnot.sl")
        .match("0", "1\n")
        .match("1", "0\n");

    test("Logical And", "land.sl")
        .match("0 0", "0\n")
        .match("1 0", "0\n")
        .match("1 1", "1\n")
        .match("0 1", "0\n");

    test("Logical Or", "lor.sl")
        .match("0 0", "0\n")
        .match("1 0", "1\n")
        .match("1 1", "1\n")
        .match("0 1", "1\n");
}