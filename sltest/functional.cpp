#include <sl/test.hpp>

sl::test::TestSuite<1> suite("Functional");

template <>
void sl::test::TestSuite<1>::run()
{
    test("Identity", "identity.sl")
        .match((values, 10), (values, 10))
        .match((values, 15), (values, 15))
        .match((values, -39), (values, -39));
}