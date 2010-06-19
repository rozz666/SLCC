#include <sl/test.hpp>

sl::test::TestSuite<7> suite("Conditional");

template <>
void sl::test::TestSuite<7>::run()
{
    test("if", "iftest.sl")
        .match((values, -5, 5), (values, 5, 5, 5, 5))
        .match((values, 5, -5), (values, 5, 5, 5, 5))
        .match((values, 10, 20), (values, 20, 20, 20, 20))
        .match((values, 20, 10), (values, 20, 20, 20, 20));

}