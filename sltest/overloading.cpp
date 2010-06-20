#include <sl/test.hpp>

sl::test::TestSuite<10> suite("Function overloading");

template <>
void sl::test::TestSuite<10>::run()
{
    test("exact", "exactoverloading.sl")
        .expect((values, 1, 2, 3, 4, 5, 6));

    test("with conversions", "overloading.sl")
        .expect((values, 7, 7, 8, 8, 8, 8, 9, 9));
}