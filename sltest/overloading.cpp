#include <sl/test.hpp>

sl::test::TestSuite<10> suite("Function overloading");

template <>
void sl::test::TestSuite<10>::run()
{
    test("exact", "exactoverloading.sl")
        .match("", "1\n2\n3\n4\n5\n6\n");

    test("with conversions", "overloading.sl")
        .match("", "7\n7\n8\n8\n8\n8\n");
}