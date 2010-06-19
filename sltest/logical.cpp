#include <sl/test.hpp>

sl::test::TestSuite<4> suite("Logical");

template <>
void sl::test::TestSuite<4>::run()
{
    test("Logical Not", "lnot.sl")
        .match((values, 0), (values, 1))
        .match((values, 1), (values, 0));

    test("Logical And", "land.sl")
        .match((values, 0, 0), (values, 0))
        .match((values, 1, 0), (values, 0))
        .match((values, 1, 1), (values, 1))
        .match((values, 0, 1), (values, 0));

    test("Logical Or", "lor.sl")
        .match((values, 0, 0), (values, 0))
        .match((values, 1, 0), (values, 1))
        .match((values, 1, 1), (values, 1))
        .match((values, 0, 1), (values, 1));
}